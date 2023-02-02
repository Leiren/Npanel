#include "imgui.h"
#include <utility>
class Color
{
private:
    Color(/* args */);
    ~Color();
public:
    static ImVec4&& copyas(const ImVec4 color){
        return std::move((ImVec4)color);
    }
    static constexpr ImVec4 green = ImVec4(0.246f, 0.836f, 0.051f, 0.65f);
    static constexpr ImVec4 orange = ImVec4(1.000f, 0.600f, 0.000f, 0.65f);
    static constexpr ImVec4 softblack =  ImVec4(0.118f, 0.118f, 0.118f, 1.000f);
    static constexpr ImVec4 red =  ImVec4(1.000f, 0.201f, 0.089f, 1.000f);

    
    static constexpr ImVec4 transparent = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
};


