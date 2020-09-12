#include "../includes/Config_parser.hpp"

Config_parser::Config_parser(char *conf) : conf(conf)
{
	line_count = 0;
}

Config_parser::~Config_parser()
{
}

void Config_parser::fail(const std::string &message)
{
	ERROR_RET("Config_parser: " + message);
	if (fd != -1)
		close(fd);
	exit(EXIT_FAILURE);
}

void Config_parser::fail_double_token(std::string &str)
{
	if (str.length() != 0)
		fail("Double token [" + std::to_string(line_count) + "]");
}

void Config_parser::setup_server()
{
	parse_conf();
	//
	for (size_t i = 0; i < serv.size(); ++i)
	{
		LOG_WRT(Logger::DEBUG, "SERVER " + std::to_string(i));
		LOG_WRT(Logger::DEBUG, "host       = " + serv[i].host);
		LOG_WRT(Logger::DEBUG, "port       = " + serv[i].port);
		LOG_WRT(Logger::DEBUG, "error_page = " + serv[i].error_page);
		LOG_WRT(Logger::DEBUG, "body_size  = " + serv[i].body_size);
		for (size_t y = 0; y < serv[i].loc.size(); ++y)
		{
			LOG_WRT(Logger::DEBUG, "LOCATION " + std::to_string(y));
			LOG_WRT(Logger::DEBUG, "	uri    = " + serv[i].loc[y].uri);
			for (size_t z = 0; z < serv[i].loc[y].method.size(); z++)
				LOG_WRT(Logger::DEBUG, "	method = " + serv[i].loc[y].method[z]);
			LOG_WRT(Logger::DEBUG, "	root   = " + serv[i].loc[y].root);
			LOG_WRT(Logger::DEBUG, "	index  = " + serv[i].loc[y].index);
			LOG_WRT(Logger::DEBUG, "	cgi    = " + serv[i].loc[y].cgi);
		}
		LOG_WRT(Logger::DEBUG, "");
	}
}

void Config_parser::parse_conf()
{
	int			ret;
	char		*cline;
	std::string	line;

	if (conf == NULL)
		fail("unvalib file");
	if ((fd = open(conf, O_RDONLY)) == -1)
		fail("fail to open config file");
	while ((ret = get_next_line(fd, &cline)) && ++line_count)
	{
		line = cline;
		free(cline);cline=NULL;
		LOG_WRT(Logger::DEBUG, "Conf    [" + std::to_string(line_count) + "] " + line);
		line.erase(std::remove_if(line.begin(), line.end(), utils_tmp::isspace), line.end());
		if (line.length() == 7 && line.compare("server{") == 0)
			parse_server();
		else if (line.length() != 0)
			fail("bad syntax [" + std::to_string(line_count) + "]");	
	}
	if (cline)
		free(cline);
	if (ret < 0)
		fail("fail to read config file");
}

void Config_parser::parse_server()
{
	int			ret;
	char		*cline;
	std::string	line;
	t_serv		new_serv;

	while ((ret = get_next_line(fd, &cline)) && ++line_count)
	{
		line = cline;
		//erase_semicol(line); if { or } return
		free(cline);cline=NULL;
		LOG_WRT(Logger::DEBUG, "Server  [" + std::to_string(line_count) + "] " + line);
		std::vector<std::string> tokens= utils_tmp::split_string(line, WHITE_SPACE);
		if (tokens.size() == 1 && tokens[0] == "}")
		{
			serv.push_back(new_serv);
			return ;
		}
		else if (tokens.size() == 0 || tokens[0][0] == '#')
			continue ;
		else if (tokens[0] == "location")
			parse_location(tokens, new_serv);
		else 
			add_serv_values(tokens, new_serv);
	}
	if (cline)
		free(cline);
	if (ret < 0)
		fail("fail to read config file");
	fail("Bracket not close [" + std::to_string(line_count) + "]");
}

void Config_parser::parse_location(std::vector<std::string> &token, t_serv &serv)
{
	int			ret;
	char		*cline;
	std::string	line;
	t_loc		new_loc;

	if (!(token.size() == 3 && token[2][0] == '{')
		&& !(token.size() == 2 && token[1][token[1].length() - 1] == '{'))
		fail("Bad location: " + line + "[" + std::to_string(line_count) + "]");
	new_loc.uri = token[1];
	while ((ret = get_next_line(fd, &cline)) && ++line_count)
	{
		line = cline;
		free(cline);cline=NULL;
		LOG_WRT(Logger::DEBUG, "Location[" + std::to_string(line_count) + "] " + line);
		std::vector<std::string> tokens= utils_tmp::split_string(line, WHITE_SPACE);
		if (tokens.size() == 1 && tokens[0] == "}")
		{
			serv.loc.push_back(new_loc);
			return ;
		}
		else if (tokens.size() == 0 || tokens[0][0] == '#')
			continue ;
		else
			add_loc_values(tokens, new_loc);
	}
	if (cline)
		free(cline);
	if (ret < 0)
		fail("fail to read config file");
	fail("Bracket not close [" + std::to_string(line_count) + "]");
}

void Config_parser::add_serv_values(std::vector<std::string> &tokens, t_serv &serv)
{
	if (tokens.size() == 1)
		fail("no arguments given [" + std::to_string(line_count) + "]");
	if (tokens.size() > 2 && tokens[2][0] != '#')
		fail("to many arguments [" + std::to_string(line_count) + "]");

	if (tokens[0] == _HOST)
	{
		fail_double_token(serv.host);
		serv.host = tokens[1];
	}	
	else if (tokens[0] == _PORT)
	{
		fail_double_token(serv.port);
		serv.port = tokens[1];
	}
	else if (tokens[0] == _ERROR_PAGE)
	{
		fail_double_token(serv.error_page);
		serv.error_page = tokens[1];
	}
	else if (tokens[0] == _BODY_SIZE)
	{
		fail_double_token(serv.body_size);
		serv.body_size = tokens[1];
	}
	else
		fail("Token invalid (" + tokens[0] + ") [" + std::to_string(line_count) + "]");
}

void Config_parser::add_loc_values(std::vector<std::string> &tokens, t_loc &loc)
{
	if (tokens.size() == 1)
		fail("no arguments given [" + std::to_string(line_count) + "]");

	if (tokens[0] == _METHOD)
	{
		fail_double_token(loc.root);
		for (size_t i = 1; i < tokens.size(); ++i)
			loc.method.push_back(tokens[i]);
	}
	else
	{
		if (tokens.size() > 2 && tokens[2][0] != '#')
			fail("to many arguments [" + std::to_string(line_count) + "]");
		if (tokens[0] == _ROOT)
		{
			fail_double_token(loc.root);
			loc.root = tokens[1];
		}
		else if (tokens[0] == _INDEX)
		{
			fail_double_token(loc.index);
			loc.index = tokens[1];
		}
		else if (tokens[0] == _CGI)
		{
			fail_double_token(loc.cgi);
			loc.cgi = tokens[1];
		}
		else
			fail("Token invalid (" + tokens[0] + ") [" + std::to_string(line_count) + "]");
	}
}