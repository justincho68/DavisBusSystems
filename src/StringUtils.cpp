#include "StringUtils.h"
#include <cctype>
#include <vector>
#include <string>
#include <algorithm>

namespace StringUtils{

std::string Slice(const std::string &str, ssize_t start, ssize_t end) noexcept{
    // Replace code here
	//Check for all cases that are not valid
	if (end == 0) {
		end = str.length();
	}
	if (str.empty() || end <= start || end <= 0 || start >= str.length()) {
		return "";
	}
    return str.substr(start, end-start);
}

std::string Capitalize(const std::string &str) noexcept{
    // Replace code here
	if (str.empty()) {
		return "";
	}
    auto Copy = str;
    Copy[0] = toupper(str[0]);
    for(size_t Index = 1; Index < Copy.length(); Index++) {
        Copy[Index] = tolower(Copy[Index]);
    }
    return Copy;
}

std::string Upper(const std::string &str) noexcept{
    auto Copy = str;
    for (size_t Index = 0; Index < Copy.length(); Index++) {
        Copy[Index] = toupper(Copy[Index]);
    }
    return Copy;
}

std::string Lower(const std::string &str) noexcept{
    auto Copy = str;
    for (size_t Index = 0; Index < Copy.length(); Index++) {
        Copy[Index] = tolower(Copy[Index]);
    }
    return Copy;
}

std::string LStrip(const std::string &str) noexcept{
    size_t start = 0;
    //While loop to search for the first non whitespace character
    while(start < str.length() && std::isspace(str[start])) {
        ++start;
    }
    return str.substr(start);
}

std::string RStrip(const std::string &str) noexcept{
    size_t end = str.length();

    if (end == 0) {
        return "";
    }

    end = end - 1;

    while (end > 0 && std::isspace(str[end])) {
        --end;
    }

    //Check if the last char is a space and end = 0, meaning the entire string is whitespace
    if (std::isspace(str[end]) && end == 0) {
        return "";
    }

    return str.substr(0,end + 1);
}

std::string Strip(const std::string &str) noexcept{
    // Replace code here
    std::string resultstr = RStrip(str);
    resultstr = LStrip(resultstr);
    return resultstr;
}

std::string Center(const std::string &str, int width, char fill) noexcept{
    // Replace code here
    int strLength = str.length();
    //The specified width must be greater than the string length
    if (width <= strLength) {
        return str;
    }
    //Calculate amount of left and right space needed to fill
    //For Uneven Fills, make extra go on right side
    int totalFill = width - strLength;
    int leftFill = totalFill/2;
    int rightFill = totalFill - leftFill;

    return std::string(leftFill, fill) + str + std::string(rightFill, fill);
}

std::string LJust(const std::string &str, int width, char fill) noexcept{
    // Replace code here
    // If the width is less than the string length
    int strLength = str.length();
    if (width <= strLength) {
        return str;
    }
    int numSpaces = width - strLength;
    std::string strFinal = str + std::string(numSpaces, fill);
    return strFinal;
}

std::string RJust(const std::string &str, int width, char fill) noexcept{
    // Replace code here
    int strLength = str.length();
    if (width <= strLength) {
        return str;
    }

    int numSpaces = width - strLength;
    std::string strFinal = std::string(numSpaces, fill) + str;
    return strFinal;
}

std::string Replace(const std::string &str, const std::string &old, const std::string &rep) noexcept{
    // Replace code here
    if (old.empty() || str.empty()) {
        return str;
    }
    
    std::string result = str;
    size_t start = 0;
    //Loop runs while an instance of old is found in str
    //npos is a constant signaling that nothing was found
    while ((start = result.find(old, start)) != std::string::npos) {
        //Replace old string with rep
        result.replace(start, old.length(), rep);
        //Set start to the value after the rep has been replaced
        start += rep.length();
    }
    return result;
}

std::vector< std::string > Split(const std::string &str, const std::string &splt) noexcept{
    // Replace code here
    //Create a vector of strings
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = 0;

    //Empty Case

    if (str == "") {
        return result;
    }

    //Check for the splt value in the string
    while((end = str.find(splt, start)) != std::string::npos) {
        //If the split is found, push the substring into the vector
        result.push_back(str.substr(start, end-start));
        //Move the start to past the split 
        start = end + splt.length();
    }

    //Add the final part of the string in the case where the string does not ent with the split
    if (start <= str.length()) {
        result.push_back(str.substr(start));
    }
    return result;
}

std::string Join(const std::string &str, const std::vector< std::string > &vect) noexcept{
    // Replace code here
    if (vect.empty()) {
        return "";
    }
    //Create string that will hold the complete joined string
    std::string complete;
    //Loop through the strings in the vector
    for (size_t Index = 0; Index < vect.size()-1; Index++) {
        std::string toAdd = vect[Index] + str;
        complete += toAdd;
    }
    complete += vect.back();
    return complete;
}

std::string ExpandTabs(const std::string &str, int tabsize) noexcept{
    // Replace code here
    if (tabsize <= 0) {
        tabsize = 8;
    }
    //Iterate through every character in the string
    std::string final;
	int charPosition = 0;

    for (size_t Index = 0; Index < str.length(); Index++) {
        char current = str[Index];
        if (current =='\t') {
            int spacesTaken = charPosition % tabsize;
			int replaceSpaces = tabsize - spacesTaken;
			for (int i = 0; i < replaceSpaces; i++) {
				final += ' ';
				charPosition++;
			}
        }
        else {
            final += current;
            charPosition++;
        }
    }
    return final;
}

int EditDistance(const std::string &left, const std::string &right, bool ignorecase) noexcept{
    // Replace code here
    std::string leftstr = left;
    std::string rightstr = right;

	//Case where Both strings are empty
	if (left == "" && right == "") {
		return 0;
	}

	//Case where one string is empty
	if (left == "") {
		return rightstr.length();
	}
	else if (right == "") {
		return leftstr.length();
	}

    //Convert both strings to lowercase if ignorecase = true

    if (ignorecase == true) {
        leftstr = Lower(leftstr);
        rightstr = Lower(rightstr);
    }

	//Create a matrix to hold all the characters of both strings using
	//Dynamically sized vector 
	int rows = leftstr.length();
	int cols = rightstr.length();
	std::vector<std::vector<int>> matrix(rows+1, std::vector<int>(cols+1));

	//Initialize the matrix

	for (int i = 0; i <= rows; i++) {
		for (int j = 0; j <= cols; j++) {
			if (i == 0) {
				matrix[i][j] = j;
			}
			else if (j == 0) {
				matrix[i][j] = i;
			}
			else {
            // Calculate distance when both strings are not empty
            matrix[i][j] = std::min({
				//First case when the last letters are different
                matrix[i-1][j-1] + (leftstr[i-1] != rightstr[j-1]), //Substitution
                matrix[i-1][j] + 1,   //Deletion
                matrix[i][j-1] + 1    //Insertion
            	});
			}
		}
	}


    return matrix[rows][cols];
}

};