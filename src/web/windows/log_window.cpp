#include "log_window.h"
#include "log.h"

void LogWindow::onFrame()
{

    // We take advantage of a rarely used feature: multiple calls to Begin()/End() are appending to the _same_ window.
    // Most of the contents of the window will be added by the log.Draw() call.
    // ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    // if (ImGui::SmallButton("[Debug] Add 5 entries"))
    // {
    //     static int counter = 0;
    //     const char *categories[3] = {"info", "warn", "error"};
    //     const char *words[] = {"Bumfuzzled", "Cattywampus", "Snickersnee", "Abibliophobia", "Absquatulate", "Nincompoop", "Pauciloquent"};
    //     for (int n = 0; n < 5; n++)
    //     {
    //         const char *category = categories[counter % IM_ARRAYSIZE(categories)];
    //         const char *word = words[counter % IM_ARRAYSIZE(words)];
    //         console.log("[%05d] [%s] Hello, current time is %.1f, here's a word: '%s'\n",
    //                        ImGui::GetFrameCount(), category, ImGui::GetTime(), word);
    //         counter++;
    //     }
    // }

    if (ImGui::BeginPopup("Options"))
    {
        ImGui::Checkbox("Auto-scroll", &console.AutoScroll);
        ImGui::EndPopup();
    }

    // Main window
    if (ImGui::Button("Options"))
        ImGui::OpenPopup("Options");
    ImGui::SameLine();
    bool clear = ImGui::Button("Clear");
    ImGui::SameLine();
    bool copy = ImGui::Button("Copy");
    ImGui::SameLine();
    console.Filter.Draw("Search", -100.0f);

    ImGui::Separator();

    if (ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar))
    {
        if (clear)
            console.Clear();
        if (copy)
            ImGui::LogToClipboard();

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        const char *buf = console.Buf.begin();
        const char *buf_end = console.Buf.end();
        if (console.Filter.IsActive())
        {
            // In this example we don't use the clipper when Filter is enabled.
            // This is because we don't have random access to the result of our filter.
            // A real application processing logs with ten of thousands of entries may want to store the result of
            // search/filter.. especially if the filtering function is not trivial (e.g. reg-exp).
            for (int line_no = 0; line_no < console.LineOffsets.Size; line_no++)
            {
                const char *line_start = buf + console.LineOffsets[line_no];
                const char *line_end = (line_no + 1 < console.LineOffsets.Size) ? (buf + console.LineOffsets[line_no + 1] - 1) : buf_end;
                if (console.Filter.PassFilter(line_start, line_end))
                    ImGui::TextUnformatted(line_start, line_end);
            }
        }
        else
        {
            // The simplest and easy way to display the entire buffer:
            //   ImGui::TextUnformatted(buf_begin, buf_end);
            // And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward
            // to skip non-visible lines. Here we instead demonstrate using the clipper to only process lines that are
            // within the visible area.
            // If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them
            // on your side is recommended. Using ImGuiListClipper requires
            // - A) random access into your data
            // - B) items all being the  same height,
            // both of which we can handle since we have an array pointing to the beginning of each line of text.
            // When using the filter (in the block of code above) we don't have random access into the data to display
            // anymore, which is why we don't use the clipper. Storing or skimming through the search result would make
            // it possible (and would be recommended if you want to search through tens of thousands of entries).
            ImGuiListClipper clipper;
            clipper.Begin(console.LineOffsets.Size);
            while (clipper.Step())
            {
                for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                {
                    const char *line_start = buf + console.LineOffsets[line_no];
                    const char *line_end = (line_no + 1 < console.LineOffsets.Size) ? (buf + console.LineOffsets[line_no + 1] - 1) : buf_end;
                    ImGui::TextUnformatted(line_start, line_end);
                }
            }
            clipper.End();
        }
        ImGui::PopStyleVar();
        if (copy)
            ImGui::LogFinish();
        // Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
        // Using a scrollbar or mouse-wheel will take away from the bottom edge.
        if (console.AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();

    // Actually call in the regular Log helper (which will Begin() into the same window as we just did)
    // log.Draw("Example: Log", p_open);
}
