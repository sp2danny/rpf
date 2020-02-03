
#include <cassert>
#include <vector>
#include <string>
#include <limits>
#include <iostream>

#include <boost/numeric/conversion/converter.hpp>

#include "purgecomment.h"
#include "common.h"

using boost::numeric::converter;

static char get(const std::string& str, int idx)
{
	if (idx < 0) return 0;
	int sz = (int)str.size();
	if (idx == sz) return '\n';
	if (idx > sz) return 0;
	return str[idx];
}

void purgeComment(std::string fn, std::vector<std::string>& vs)
{
	bool in_block_comment = false;
	bool in_line_comment  = false;
	bool in_string        = false;
	bool in_char          = false;

	int ln = 0;
	for (std::string& oldline : vs)
	{
		while (true)
		{
			auto pos = oldline.find_first_of("\n\r");
			if (pos == std::string::npos) break;
			oldline.erase(pos, 1);
		}
		std::string line = oldline;

		++ln;
		if (line.empty()) continue;
		int i = 0;
		converter<int, std::size_t> conv;
		assert(!conv.out_of_range(line.size()));
		int n = conv(line.size());
		char curr, next, prev;
		while (true)
		{
			curr = get(oldline, i  );
			next = get(oldline, i+1);
			prev = get(oldline, i-1);

			/**/ if (in_line_comment)
			{
				if (i >= n) {
					in_line_comment = false;
					break;
				}
				line[i] = ' ';
				++i;
			}
			else if (in_string)
			{
				if (i >= n)
				{
					if (prev == '\\')
						break;
					if (runstate::warnings)
						std::cerr << "warning: string not teminated, " << fn << " : " << ln << std::endl;
					in_string = false;
					break;
				}
				if (curr == '\\')
				{
					line[i++] = ' ';
					if (i >= n)
						break;
				}
				else if (curr == '"')
				{
					in_string = false;
				}
				line[i++] = ' ';
			}
			else if (in_char)
			{
				if (i >= n)
				{
					if (runstate::warnings)
						std::cerr << "warning: char not teminated, " << fn << " : " << ln << std::endl;
					in_char = false;
					break;
				}
				if (curr == '\'')
				{
					if (prev != '\\')
						in_char = false;
					else
						if (get(oldline, i-2) == '\\')
							in_char = false;
				}
				line[i] = ' ';
				++i;
			}
			else if (in_block_comment)
			{
				if (i >= n) break;
				if (curr == '/')
					if (prev == '*')
						in_block_comment = false;
				line[i] = ' ';
				++i;
			}
			else
			{
				if (i >= n) break;
				switch (curr)
				{
				case '/':
					switch (next)
					{
					case '*':
						in_block_comment = true;
						line[i++] = ' ';
						break;
					case '/':
						in_line_comment = true;
						line[i++] = ' ';
						break;
					default:
						++i;
					}
					break;
				case '"':
					in_string = true;
					line[i++] = ' ';
					break;
				case '\'':
					in_char = true;
					line[i++] = ' ';
					break;
				default:
					++i;
				}
			}
		}
		oldline = line;
	}
}

