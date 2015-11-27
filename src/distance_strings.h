#ifndef _DISTANCE_STRINGS_H_
#define _DISTANCE_STRINGS_H_

#include <algorithm>
#include <vector>
#include <string>
#include <math.h>
#include <set>
#include "common.h"

/*
	Levenshtein distance: string metric for
	measuring the difference between two sequences
	Levenshtein is a measure of dissimilarity

	This implementation uses bottom-up dynamic programming.

	Reference:
		http://www.levenshtein.net/

	Return:
		score of dissimilarity
*/
int levenshteinDistance(std::string & s1, std::string & s2)
{
	int size_s1 = s1.size(), size_s2 = s2.size();
	std::vector<std::vector<unsigned int> > mat(size_s1 + 1,
			std::vector<unsigned int>(size_s2 + 1));

	mat[0][0] = 0;

	for(int i = 1; i <= size_s1; i++)
		mat[i][0] = i;
	for(int i = 1; i <= size_s2; i++)
		mat[0][i] = i;

	for(int i = 1; i <= size_s1; i++)
		for(int j = 1; j <= size_s2; j++)
			mat[i][j] = std::min(std::min(mat[i - 1][j] + 1, mat[i][j - 1] + 1),
								 mat[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1));

	return mat[size_s1][size_s2];
}

/*
	Needleman-Wunsch is a algorithm for global alignment pairwise.
	Needleman-Wunsch is a measure of similarity.
	Uses dynamic programming.

	References:
		https://en.wikipedia.org/wiki/Needleman�Wunsch_algorithm
		http://pt.slideshare.net/mcastrosouza/algoritmo-needlemanwunsch (in portuguese)

	Parameters:
		match
		mistmatch
		gap: score to give an insertion or deletion mutation

	Return:
		score of the alignment
*/
double needlemanWunsch(std::string & s1, std::string & s2, double match = 1,
					   double mismatch = 0, double gap = 0)
{
	int size_s1 = s1.size(), size_s2 = s2.size();
	std::vector<std::vector<double> > mat(size_s1 + 1,
										  std::vector<double>(size_s2 + 1));

	mat[0][0] = 0;

	for(int i = 1; i <= size_s1; i++)
		mat[i][0] = mat[i - 1][0] + gap;
	for(int i = 1; i <= size_s2; i++)
		mat[0][i] = mat[0][i - 1] + gap;

	for(int i = 1; i <= size_s1; i++)
	{
		for(int j = 1; j <= size_s2; j++)
			mat[i][j] = std::max(std::max(mat[i - 1][j] + gap, mat[i][j - 1] + gap),
								 mat[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? match : mismatch));
	}

	return mat[size_s1][size_s2];
}

/*
	White Similarity (similarity ranking)
	reference: http://www.catalysoft.com/articles/StrikeAMatch.html
	is the same Dice's Coefficient
*/

double whiteSimilarity(std::string & s1, std::string & s2, int window = 5)
{
	std::set<std::string> s1_bigrams;
	std::set<std::string> s2_bigrams;

	int size_s1 = s1.size(), size_s2 = s2.size();

	// base case
	if(size_s1 == 0 || size_s2 == 0)
		return 0;
	else if(s1 == s2)
		return 100;

	std::string sub_str;

	// extract characters of length "window" from s1
	for(int i = 0; i < (size_s1 - 1); i++)
	{
		sub_str = s1.substr(i, window);
		if(sub_str.size() == (unsigned)window)
			s1_bigrams.insert(sub_str);
		else
			break;
	}

	// extract characters of length "window" from s2
	for(int i = 0; i < (size_s2 - 1); i++)
	{
		sub_str = s2.substr(i, window);
		if(sub_str.size() == (unsigned)window)
			s2_bigrams.insert(sub_str);
		else
			break;
	}

	int intersection = 0;

	// find the intersection between the two sets

	std::set<std::string>::iterator it;

	for(it = s2_bigrams.begin(); it != s2_bigrams.end(); it++)
		intersection += s1_bigrams.count((*it));

	int total = s1_bigrams.size() + s2_bigrams.size();

	return (((intersection * 2.0) / total) * 100);
}

/*
	Similarity based on KMP (Knuth-Morris-Pratt) algorithm.

	reference (in portuguese):
		http://pt.slideshare.net/mcastrosouza/algoritmo-de-knuthmorrispratt-kmp
*/
double kmp(std::string & s1, std::string & s2, int window = 5)
{
	double score = 0;

	// base case
	if(s1 == "" || s2 == "")
		return score;

	int size_s1 = s1.size(), size_s2 = s2.size();
	
	if(size_s1 < window)
		return 0;
	
	std::vector<std::string> words;
	std::string sub_str;

	// extract words of the s1
	for(int i = 0; i < (size_s1 - 1); i++)
	{
		sub_str = s1.substr(i, window);
		if(sub_str.size() == (unsigned)window)
			words.push_back(sub_str);
		else
			break;
	}

	int size_words = words.size();

	// for each pattern (word)
	for(int k = 0; k < size_words; k++)
	{
		// compute auxiliary vector (largest prefix that is also suffix)
		std::vector<int> aux(window);

		aux[0] = 0;
		int j = 0, i = 1;

		while(i < window)
		{
			if(words[k][i] == words[k][j])
			{
				j++;
				aux[i] = j;
				i++;
			}
			else
			{
				if(j)
					j = aux[j - 1];
				else
				{
					aux[i] = 0;
					i++;
				}
			}
		}

		int idx_text = 0, idx_pattern = 0;

		// searches the pattern in the text
		while(idx_text < size_s2)
		{
			if(words[k][idx_pattern] == s2[idx_text])
			{
				idx_pattern++;
				idx_text++;
			}

			if(idx_pattern == window)
			{
				score++;
				idx_pattern = aux[idx_pattern - 1];
			}

			if(idx_text < size_s2 &&
					words[k][idx_pattern] != s2[idx_text])
			{
				if(idx_pattern)
					idx_pattern = aux[idx_pattern - 1];
				else
					idx_text++;
			}
		}
	}

	return score;
}


#endif
