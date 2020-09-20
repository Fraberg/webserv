#include "../includes/utils_tmp.hpp"

std::string	utils_tmp::get_date(void)
{
    struct timeval 	tv;
    struct tm 		time;
    struct timezone tz;
    char 			buffer[1000];
    std::string 	date;

    gettimeofday(&tv, &tz);
    strptime(std::to_string(tv.tv_sec).c_str(), "%s", &time);
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S CEST", &time);
    date = buffer;
	return (date);
}

int utils_tmp::isspace(int c)
{
	char space[] = "\t\n\v\f\r ";
	int i = -1;

	while (++i < 6)
		if ((char)c == space[i])
			return (1);
	return (0);
}

std::vector<std::string> utils_tmp::split_string(std::string &str, std::string set)
{
	std::vector<std::string> split;
	std::size_t p1;
	std::size_t p2 = 0;
	
	while (p2 < str.length())
	{
		if ((p1 = str.find_first_not_of(set, p2)) == std::string::npos)
			return (split);
		if ((p2 = str.find_first_of(set, p1)) == std::string::npos)
			p2 = str.length();
		split.push_back(str.substr(p1, (p2 - p1)));
	}
	return (split);
}

bool utils_tmp::file_exists(const char *filename)
{
	struct stat	buffer;
	return (stat (filename, &buffer) == 0);
}

int	utils_tmp::read_file(int fd, std::string file, std::string &buff)
{
	int ret;
	char *cline;
	std::string line;

	if ((fd = open(file.c_str(), O_RDONLY|O_NONBLOCK, 0666)) == -1)//mettre dans read file
		return (-1);
	while ((ret = get_next_line(fd, &cline)))
	{
		line = cline;
		free(cline);
		cline=NULL;
		buff += line + "\n";
	}
	if (cline)
	{
		line = cline;
		free(cline);
		buff += line;
	}
	if (ret < 0)
		return (-1);
	return (0);
}

std::string utils_tmp::extract_body(std::string &buff)
{
	std::string res;
	res = buff.erase(0 , buff.find_last_of("\n\r\n") + 1);
	return (res);
}