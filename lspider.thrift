namespace cpp lspider

struct IUrl {
    1: string str,
    2: string ip,
    3: bool hub = false,
}

service CrawlService {
    void request(1: IUrl u),
    string exec_cmd(1: string cmd),
}
