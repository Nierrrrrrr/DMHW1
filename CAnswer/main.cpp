#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <iomanip>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/fstream.hpp>

using namespace boost::multiprecision;
using namespace boost::algorithm;
using namespace std;

multimap<string, float> movie_score_map, user_score_map;

bool parse_movie_file(ifstream & ifs)
{
	string productId, userId;
	float score;

	bool have_data = false;
	string line;
	char tmp[102400];
	ifs.getline(tmp, 102400);
	line = tmp;


	int count = 0;
	int line_count = 0;
	while (ifs.eof() == false)
	{
		if (line == "" && have_data)
		{
			// insert scores
			movie_score_map.insert(pair<string, float>(productId, score));
			user_score_map.insert(pair<string, float>(userId, score));
			have_data = false;
		}
		else if (line.find("productId") != string::npos)
		{
			productId = line.substr(line.find(":") + 1);
			trim(productId);

			if (productId == "B004LIYF62")
				cout << line_count << endl;
		}
		else if (line.find("userId") != string::npos)
		{
			userId = line.substr(line.find(":") + 1);
			trim(userId);
		}
		else if (line.find("review/score:") != string::npos)
		{
			score = atof(line.substr(line.find(":") + 1).c_str());
			have_data = true;
		}

		ifs.getline(tmp, 102400);
		line = tmp;
		if (++line_count % 10000 == 0)
			cout << line_count << endl;
	}
	return true;
}

bool output(ofstream & ofs, multimap<string, float> & mmap)
{
	cpp_dec_float_100 total_movie_sum = 0.0;
	cpp_dec_float_100 total_movie_square_sum = 0.0;
	int total_movie_count = 0;
	string current_movie = "";

	for (auto it = mmap.begin(); it != mmap.end(); ++it)
	{
		current_movie = it->first;
		float movie_sum = 0.0;
		float movie_square_sum = 0.0;
		int movie_count = 0;
		do {
			movie_sum += it->second;
			movie_square_sum += it->second * it->second;
			++movie_count;
			++it;
		} while (it != mmap.end() && current_movie == it->first);
		--it;

		float movie_mean = movie_sum / movie_count;
		float movie_variance = movie_square_sum / movie_count - movie_mean * movie_mean;
		ofs << current_movie << " " << fixed << setprecision(3) << movie_mean << " " << movie_variance << endl;
		total_movie_sum += movie_mean;
		total_movie_square_sum += movie_mean * movie_mean;
		++total_movie_count;
	}

	cpp_dec_float_100 total_movie_mean = total_movie_sum / total_movie_count;
	cpp_dec_float_100 total_movie_variance = total_movie_square_sum / total_movie_count - total_movie_mean * total_movie_mean;
	ofs << fixed << setprecision(3) << total_movie_mean << " " << total_movie_variance << endl;
	return true;
}

int main(void)
{
	// open file
	ifstream ifs("F:\\movies.txt", ifstream::in);
	if (ifs.is_open() == false)
	{
		cerr << "Input movies file cannot be opened." << endl;
		return -1;
	}

	// parse file
	if (parse_movie_file(ifs) == false)
	{
		cerr << "Error at parse_movie_file." << endl;
		return -2;
	}

	ifs.close();

	// output
	ofstream ofs_movie("answer_movie.txt", ofstream::out);
	output(ofs_movie, movie_score_map);
	ofs_movie.close();

	ofstream ofs_user("answer_user.txt", ofstream::out);
	output(ofs_user, user_score_map);
	ofs_user.close();
	
	return 0;
}