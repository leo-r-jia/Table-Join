#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <cassert>

using namespace std;

// a table with a header (row of attribute names)
struct table
{
	// attribute names
	vector<string> names;

	// two-dimensional table of data
	vector< vector<string> > data;

	// extraction operator for reading a table from file
	friend istream& operator>> (istream& in, table&);
};

// splits a tab-separated string of elements into a vector of elements
void split(string str, char delim, vector<string>& elems)
{
	while (str.length())
	{
		string item;

		string::size_type d = str.find(delim);
		if (d == string::npos)
		{
			// end of line reached
			item = str;
			str = "";
		}
		else
		{
			item = str.substr(0, d);
			str.erase(0, d + 1);
		}
		elems.push_back(item);
	}
}

istream& operator>> (istream& in, table& tab)
{
	// parse the header (attribute names)
	string line;
	std::getline(in, line);
	split(line, '\t', tab.names);

	// parse the table of rows
	while (std::getline(in, line))
	{
		vector<string> row_values;
		split(line, '\t', row_values);

		// each row must have the same number of columns as the header
		assert(row_values.size() == tab.names.size());

		tab.data.push_back(row_values);
	}

	return in;
}

// the join operation to be implemented

table join(const table& t1, const string& name1, const table& t2, const string& name2)
{
	table result;

	// find column indices corresponding to column names
	unsigned t1col = 0;
	for (unsigned i = 0; i < t1.names.size(); i++)
	{
		if (t1.names[i] == name1) {
			t1col = i;
		}
	}
	unsigned t2col = 0;
	for (unsigned i = 0; i < t2.names.size(); i++)
	{
		if (t2.names[i] == name2) {
			t2col = i;
		}
	}

	// set names vector
	result.names = t1.names;
	result.names.insert(result.names.end(), t2.names.begin(), t2.names.end());
	result.names.erase(result.names.begin() + t1col);

	// find suitable matches for join
	for (unsigned k = 0; k < t1.data.size(); k++) {
		for (unsigned j = 0; j < t2.data.size(); j++) {
			if (t1.data.at(k)[t1col] == t2.data.at(j)[t2col]) {
				// insert all t1.data and t2.data elements into vector v
				vector<string> v = t1.data.at(k);
				for (unsigned i = 0; i < t2.names.size(); i++) {
					v.insert(v.end(), t2.data.at(j)[i]);
				}
				// erase repeated element
				v.erase(v.begin() + t1col);
				// insert vector v into result.data
				result.data.push_back(v);
			}
		}
	}

	return result;
}

void longestRuntime(const table& t)
{
	unsigned index = 0;
	int length = 0;

	// find column index corresponding to run time
	unsigned minsCol = 0;
	for (unsigned i = 0; i < t.names.size(); i++)
	{
		if (t.names[i] == "runtimeMinutes") {
			minsCol = i;
		}
	}
	// find column index corresponding to movie title
	unsigned nameCol = 0;
	for (unsigned i = 0; i < t.names.size(); i++)
	{
		if (t.names[i] == "originalTitle") {
			nameCol = i;
		}
	}
	// find length and index of longest movie
	for (unsigned i = 0; i < t.data.size(); i++) {
		if (stoi(t.data.at(i)[minsCol]) > length) {
			length = stoi(t.data.at(i)[minsCol]);
			index = i;
		}
	}

	cerr << "Longest runtime: " << t.data.at(index)[nameCol] << " at " << length << " minutes";
}

void longestCollectiveRuntime(const table& t)
{
	unsigned index = 0;
	int greatestLength = 0;

	// find column index corresponding to run time
	unsigned minsCol = 0;
	for (unsigned i = 0; i < t.names.size(); i++)
	{
		if (t.names[i] == "runtimeMinutes") {
			minsCol = i;
		}
	}
	// find column index corresponding to director name
	unsigned dirNameCol = 0;
	for (unsigned i = 0; i < t.names.size(); i++) {
		if (t.names[i] == "primaryName") {
			dirNameCol = i;
		}
	}

	// for each director, sum the runtime of directed movies
	for (unsigned i = 0; i < t.data.size(); i++) {
		// initialize runtime length for directed movies
		int length = stoi(t.data.at(i)[minsCol]);
		// for every other movie directed by same director, add to length
		for (unsigned j = 0; j < t.data.size(); j++) {
			if (t.data.at(i)[dirNameCol] == t.data.at(j)[dirNameCol]) {
				if (i != j) {
					length += stoi(t.data.at(j)[minsCol]);
				}
			}
		}
		// if runtime length is greater than the current greatest, set as greatest
		if (length > greatestLength) {
			index = i;
			greatestLength = length;
		}
	}

	cerr << "\nDirector with longest collective runtime: " << t.data.at(index)[dirNameCol] << " at " << greatestLength << " minutes";
}

int main()
{
	ifstream director_input("directors.tsv");

	table directors;
	director_input >> directors;

	cerr << directors.data.size() << " directors loaded" << endl;

	ifstream movie_input("romcom.tsv");

	table movies;
	movie_input >> movies;

	cerr << movies.data.size() << " romantic comedies loaded" << endl;

	table joined = join(movies, "directors", directors, "nconst");

	cerr << joined.data.size() << " rows in join" << endl;

	longestRuntime(joined);

	longestCollectiveRuntime(joined);

	return 0;
}