#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>

struct Packet {
    std::string src_ip;
    std::string src_port;
    std::string dst_ip;
    std::string dst_port;
};

%%{
    machine ip_parser;
    access _;
    
    variable cs  parser_cs;
    variable p   parser_p;
    variable pe  parser_pe;
    variable eof parser_eof;
    variable ts  parser_ts;
    
    action mark_src_ip { parser_ts = parser_p; }
    action set_src_ip { packet.src_ip.assign(parser_ts, parser_p - parser_ts); }
    action mark_src_port { parser_ts = parser_p; }
    action set_src_port { packet.src_port.assign(parser_ts, parser_p - parser_ts); }
    
    action mark_dst_ip { parser_ts = parser_p; }
    action set_dst_ip { packet.dst_ip.assign(parser_ts, parser_p - parser_ts); }
    action mark_dst_port { parser_ts = parser_p; }
    action set_dst_port { packet.dst_port.assign(parser_ts, parser_p - parser_ts); }

    ipv4 = (digit+'.'){3} digit+;
    port = digit+;
    
    src = 
        ipv4 >mark_src_ip %set_src_ip 
        '.' 
        port >mark_src_port %set_src_port;
        
    dst = 
        ipv4 >mark_dst_ip %set_dst_ip 
        '.' 
        port >mark_dst_port %set_dst_port;

    data_line = 
        space* src 
        space* '>' space* 
        dst 
        (any - '\n')*; 

    main := 
        (any* '\n')    # Пропускаем заголовок
        data_line '\n'  # Обрабатываем данные
        @{ packets.push_back(packet); packet = Packet{}; };
}%%

class Parser {
    std::vector<Packet> packets;
    Packet packet;
    int parser_cs;
    const char *parser_ts;
    const char *parser_p;
    const char *parser_pe;
    const char *parser_eof;

public:
    Parser();
    void parse(const char* data, size_t len);
    void to_csv(const std::string& filename) const;
};

%% write data;

Parser::Parser() {
    %% write init;
    parser_cs = ip_parser_start;
    parser_ts = parser_p = parser_pe = parser_eof = nullptr;
}

void Parser::parse(const char* data, size_t len) {
    parser_p = data;
    parser_pe = data + len;
    parser_eof = parser_pe;
    
    %% write exec;
}

void Parser::to_csv(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error creating file: " << filename << "\n";
        return;
    }
    
    file << "src_ip,src_port,dst_ip,dst_port\n";
    for(const auto& p : packets) {
        file << p.src_ip << "," 
             << p.src_port << "," 
             << p.dst_ip << "," 
             << p.dst_port << "\n";
    }
}

int main(int argc, char** argv) {
    if(argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output.csv>\n";
        return 1;
    }

    Parser parser;
    std::ifstream file(argv[1]);
    std::string packet_block;
    std::string line;
    auto start_time = std::chrono::high_resolution_clock::now();
    while(std::getline(file, line)) {
        if(packet_block.empty()) {
            packet_block = line + "\n";
        } else {
            packet_block += line + "\n";
            parser.parse(packet_block.c_str(), packet_block.size());
            packet_block.clear();
        }
    }
    parser.to_csv(argv[2]);
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = end_time - start_time;
    std::cout << "Время выполнения: " << elapsed_time.count() << " секунд" << std::endl;
    return 0;
}