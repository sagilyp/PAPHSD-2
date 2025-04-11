
#line 1 "ip_parser.cpp"
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


#line 59 "ip_parser.cpp"


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


#line 33 "ip_parser_gen.cpp"
static const char _ip_parser_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3, 1, 4, 1, 5, 1, 6, 1, 
	7, 1, 8, 2, 0, 4, 2, 1, 
	5, 2, 2, 6, 2, 3, 7, 2, 
	7, 8, 3, 3, 7, 8
};

static const unsigned char _ip_parser_key_offsets[] = {
	0, 1, 6, 10, 13, 17, 20, 24, 
	27, 31, 34, 41, 46, 52, 58, 63, 
	67, 70, 74, 77, 81, 84, 88, 91, 
	98, 99, 104, 110, 114, 117, 121, 124, 
	128, 131, 135, 138, 141, 145, 148, 152, 
	155, 159, 162, 166, 169, 174, 180
};

static const char _ip_parser_trans_keys[] = {
	10, 32, 9, 13, 48, 57, 10, 46, 
	48, 57, 10, 48, 57, 10, 46, 48, 
	57, 10, 48, 57, 10, 46, 48, 57, 
	10, 48, 57, 10, 46, 48, 57, 10, 
	48, 57, 10, 32, 62, 9, 13, 48, 
	57, 10, 32, 62, 9, 13, 32, 62, 
	9, 13, 48, 57, 10, 32, 9, 13, 
	48, 57, 32, 9, 13, 48, 57, 10, 
	46, 48, 57, 10, 48, 57, 10, 46, 
	48, 57, 10, 48, 57, 10, 46, 48, 
	57, 10, 48, 57, 10, 46, 48, 57, 
	10, 48, 57, 10, 32, 62, 9, 13, 
	48, 57, 10, 10, 32, 62, 9, 13, 
	10, 32, 9, 13, 48, 57, 10, 46, 
	48, 57, 10, 48, 57, 10, 46, 48, 
	57, 10, 48, 57, 10, 46, 48, 57, 
	10, 48, 57, 10, 46, 48, 57, 10, 
	48, 57, 10, 48, 57, 10, 46, 48, 
	57, 10, 48, 57, 10, 46, 48, 57, 
	10, 48, 57, 10, 46, 48, 57, 10, 
	48, 57, 10, 46, 48, 57, 10, 48, 
	57, 32, 9, 13, 48, 57, 32, 62, 
	9, 13, 48, 57, 32, 9, 13, 48, 
	57, 0
};

static const char _ip_parser_single_lengths[] = {
	1, 1, 2, 1, 2, 1, 2, 1, 
	2, 1, 3, 3, 2, 2, 1, 2, 
	1, 2, 1, 2, 1, 2, 1, 3, 
	1, 3, 2, 2, 1, 2, 1, 2, 
	1, 2, 1, 1, 2, 1, 2, 1, 
	2, 1, 2, 1, 1, 2, 1
};

static const char _ip_parser_range_lengths[] = {
	0, 2, 1, 1, 1, 1, 1, 1, 
	1, 1, 2, 1, 2, 2, 2, 1, 
	1, 1, 1, 1, 1, 1, 1, 2, 
	0, 1, 2, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 2, 2, 2
};

static const unsigned char _ip_parser_index_offsets[] = {
	0, 2, 6, 10, 13, 17, 20, 24, 
	27, 31, 34, 40, 45, 50, 55, 59, 
	63, 66, 70, 73, 77, 80, 84, 87, 
	93, 95, 100, 105, 109, 112, 116, 119, 
	123, 126, 130, 133, 136, 140, 143, 147, 
	150, 154, 157, 161, 164, 168, 173
};

static const char _ip_parser_indicies[] = {
	1, 0, 1, 1, 2, 0, 1, 3, 
	4, 0, 1, 5, 0, 1, 6, 5, 
	0, 1, 7, 0, 1, 8, 7, 0, 
	1, 9, 0, 1, 10, 9, 0, 1, 
	11, 0, 13, 12, 15, 12, 14, 0, 
	17, 16, 18, 16, 0, 17, 18, 17, 
	2, 0, 19, 18, 18, 20, 0, 19, 
	19, 21, 0, 1, 22, 23, 0, 1, 
	24, 0, 1, 25, 24, 0, 1, 26, 
	0, 1, 27, 26, 0, 1, 28, 0, 
	1, 29, 28, 0, 1, 30, 0, 33, 
	32, 35, 32, 34, 31, 37, 36, 39, 
	38, 40, 38, 36, 41, 40, 40, 42, 
	36, 37, 43, 44, 36, 37, 45, 36, 
	37, 46, 45, 36, 37, 47, 36, 37, 
	48, 47, 36, 37, 49, 36, 37, 50, 
	49, 36, 37, 51, 36, 52, 53, 31, 
	1, 54, 55, 0, 1, 56, 0, 1, 
	57, 56, 0, 1, 58, 0, 1, 59, 
	58, 0, 1, 60, 0, 1, 61, 60, 
	0, 1, 51, 0, 1, 1, 2, 0, 
	17, 18, 17, 2, 0, 19, 19, 21, 
	0, 0
};

static const char _ip_parser_trans_targs[] = {
	0, 1, 2, 3, 2, 4, 5, 6, 
	7, 8, 9, 10, 11, 12, 10, 13, 
	11, 12, 13, 14, 36, 15, 16, 15, 
	17, 18, 19, 20, 21, 22, 23, 24, 
	25, 45, 23, 26, 24, 44, 25, 45, 
	26, 46, 27, 28, 27, 29, 30, 31, 
	32, 33, 34, 35, 44, 35, 37, 36, 
	38, 39, 40, 41, 42, 43
};

static const char _ip_parser_trans_actions[] = {
	0, 0, 1, 0, 0, 0, 0, 0, 
	0, 0, 3, 5, 7, 7, 0, 7, 
	0, 0, 0, 0, 9, 19, 0, 0, 
	0, 0, 0, 0, 0, 22, 25, 15, 
	28, 34, 15, 28, 0, 17, 0, 17, 
	0, 17, 9, 0, 0, 0, 0, 0, 
	0, 0, 11, 13, 31, 15, 0, 0, 
	0, 0, 0, 0, 0, 11
};

static const int ip_parser_start = 0;
static const int ip_parser_first_final = 44;
static const int ip_parser_error = -1;

static const int ip_parser_en_main = 0;


#line 77 "ip_parser.cpp"

Parser::Parser() {
    
#line 160 "ip_parser_gen.cpp"
	{
	(  parser_cs) = ip_parser_start;
	}

#line 80 "ip_parser.cpp"
    parser_cs = ip_parser_start;
    parser_ts = parser_p = parser_pe = parser_eof = nullptr;
}

void Parser::parse(const char* data, size_t len) {
    parser_p = data;
    parser_pe = data + len;
    parser_eof = parser_pe;
    
    
#line 172 "ip_parser_gen.cpp"
	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const char *_keys;

	if ( (   parser_p) == (  parser_pe) )
		goto _test_eof;
_resume:
	_keys = _ip_parser_trans_keys + _ip_parser_key_offsets[(  parser_cs)];
	_trans = _ip_parser_index_offsets[(  parser_cs)];

	_klen = _ip_parser_single_lengths[(  parser_cs)];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( (*(   parser_p)) < *_mid )
				_upper = _mid - 1;
			else if ( (*(   parser_p)) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (unsigned int)(_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _ip_parser_range_lengths[(  parser_cs)];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( (*(   parser_p)) < _mid[0] )
				_upper = _mid - 2;
			else if ( (*(   parser_p)) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += (unsigned int)((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _ip_parser_indicies[_trans];
	(  parser_cs) = _ip_parser_trans_targs[_trans];

	if ( _ip_parser_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _ip_parser_actions + _ip_parser_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
#line 25 "ip_parser.cpp"
	{ parser_ts = parser_p; }
	break;
	case 1:
#line 26 "ip_parser.cpp"
	{ packet.src_ip.assign(parser_ts, parser_p - parser_ts); }
	break;
	case 2:
#line 27 "ip_parser.cpp"
	{ parser_ts = parser_p; }
	break;
	case 3:
#line 28 "ip_parser.cpp"
	{ packet.src_port.assign(parser_ts, parser_p - parser_ts); }
	break;
	case 4:
#line 30 "ip_parser.cpp"
	{ parser_ts = parser_p; }
	break;
	case 5:
#line 31 "ip_parser.cpp"
	{ packet.dst_ip.assign(parser_ts, parser_p - parser_ts); }
	break;
	case 6:
#line 32 "ip_parser.cpp"
	{ parser_ts = parser_p; }
	break;
	case 7:
#line 33 "ip_parser.cpp"
	{ packet.dst_port.assign(parser_ts, parser_p - parser_ts); }
	break;
	case 8:
#line 58 "ip_parser.cpp"
	{ packets.push_back(packet); packet = Packet{}; }
	break;
#line 270 "ip_parser_gen.cpp"
		}
	}

_again:
	if ( ++(   parser_p) != (  parser_pe) )
		goto _resume;
	_test_eof: {}
	}

#line 90 "ip_parser.cpp"
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