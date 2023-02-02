#include "server_report_store.h"

rocket::signal<void(ServerReport *)> ServerReportStore::signal;
ServerReport ServerReportStore::last_report;

