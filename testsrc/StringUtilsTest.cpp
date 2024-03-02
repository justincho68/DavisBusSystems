#include <gtest/gtest.h>
#include "StringUtils.h"

TEST(StringUtilsTest, SliceTest){
    std::string Base = "Hello";

    EXPECT_EQ(StringUtils::Slice(Base,0,Base.length()),"Hello");
    EXPECT_EQ(StringUtils::Slice(Base,1,Base.length()),"ello");
    EXPECT_EQ(StringUtils::Slice(Base,1),"ello");
    EXPECT_EQ(StringUtils::Slice("",0,5), "");
    EXPECT_EQ(StringUtils::Slice("Justin",-4,2), "");
    EXPECT_EQ(StringUtils::Slice("Justin",1,25), "ustin");
}

TEST(StringUtilsTest, Capitalize){
    std::string Base = "hello";

    EXPECT_EQ(StringUtils::Capitalize(Base),"Hello");
    //Empty Case
    EXPECT_EQ(StringUtils::Capitalize(std::string()),"");
    EXPECT_EQ(StringUtils::Capitalize(" hello")," hello");
    //All Caps case becomes only first char capitalized
    EXPECT_EQ(StringUtils::Capitalize("HELLO"),"Hello");
    //Single Character case
    EXPECT_EQ(StringUtils::Capitalize("h"),"H");
    //No letters case
    EXPECT_EQ(StringUtils::Capitalize("1234"),"1234");
    //Letters that are not the first char are capitalized
    EXPECT_EQ(StringUtils::Capitalize("hELLo"),"Hello");
}

TEST(StringUtilsTest, Upper){
    EXPECT_EQ(StringUtils::Upper("Hello World"), "HELLO WORLD");
    EXPECT_EQ(StringUtils::Upper(""),"");
    EXPECT_EQ(StringUtils::Upper("HELLO WORLD"),"HELLO WORLD");
    EXPECT_EQ(StringUtils::Upper(" Hello World "), " HELLO WORLD ");

}

TEST(StringUtilsTest, Lower){
    EXPECT_EQ(StringUtils::Lower("Hello World"), "hello world");
    EXPECT_EQ(StringUtils::Lower(""), "");
    EXPECT_EQ(StringUtils::Lower("hello world"), "hello world");
    EXPECT_EQ(StringUtils::Lower(" Hello World "), " hello world ");
}

TEST(StringUtilsTest, LStrip){
    EXPECT_EQ(StringUtils::LStrip(" Hello World"), "Hello World");
    EXPECT_EQ(StringUtils::LStrip("\t Hello World"), "Hello World");
    EXPECT_EQ(StringUtils::LStrip("\t\n Hello World"), "Hello World");
}

TEST(StringUtilsTest, RStrip){
    EXPECT_EQ(StringUtils::RStrip("Hello World "), "Hello World");
    EXPECT_EQ(StringUtils::RStrip("Hello World \t"), "Hello World");
    EXPECT_EQ(StringUtils::RStrip("Hello World \t\n"), "Hello World");
}

TEST(StringUtilsTest, Strip){
    EXPECT_EQ(StringUtils::Strip(" Hello World "), "Hello World");
    EXPECT_EQ(StringUtils::Strip("\t Hello World \t"), "Hello World");
    EXPECT_EQ(StringUtils::Strip("\t\n Hello World \t\n"), "Hello World");
}

TEST(StringUtilsTest, Center){
    EXPECT_EQ(StringUtils::Center("Hello",5,' '), "Hello");
    EXPECT_EQ(StringUtils::Center("Hello",7,' '), " Hello ");
    EXPECT_EQ(StringUtils::Center("Hello",6,' '), "Hello ");
    EXPECT_EQ(StringUtils::Center("Hello", 3,' '), "Hello");
    EXPECT_EQ(StringUtils::Center("x",3,'-'), "-x-");
    EXPECT_EQ(StringUtils::Center("", 2, ' '), "  ");
}

TEST(StringUtilsTest, LJust){
    EXPECT_EQ(StringUtils::LJust("Hello", 5, ' '), "Hello");
    EXPECT_EQ(StringUtils::LJust("Hello", 7, ' '), "Hello  ");
    EXPECT_EQ(StringUtils::LJust("Hello", 9, '-'), "Hello----");
    EXPECT_EQ(StringUtils::LJust("", 5, '-'), "-----");

}

TEST(StringUtilsTest, RJust){
    EXPECT_EQ(StringUtils::RJust("Hello",5,' '), "Hello");
    EXPECT_EQ(StringUtils::RJust("Hello",7,' '), "  Hello");
    EXPECT_EQ(StringUtils::RJust("Hello",9,'-'), "----Hello");
    EXPECT_EQ(StringUtils::RJust("",5,'-'), "-----");
}

TEST(StringUtilsTest, Replace){
    EXPECT_EQ(StringUtils::Replace("Hello Justin Cho", "Justin", "Friend"), "Hello Friend Cho");
    EXPECT_EQ(StringUtils::Replace("a a a", "a", "b"), "b b b");
    EXPECT_EQ(StringUtils::Replace("Hello World", "Justin", "David"), "Hello World");
    EXPECT_EQ(StringUtils::Replace("", "Justin", "David"), "");
    EXPECT_EQ(StringUtils::Replace("Hello World", "lo", "Goodbye"), "HelGoodbye World");
    EXPECT_EQ(StringUtils::Replace("Justin Justin justin", "justin", "David"), "Justin Justin David");
}

TEST(StringUtilsTest, Split){
    //Assigning everything to variables because GTEST errors when putting on one line
    auto actual = StringUtils::Split("David,Emily,Justin", ",");
    std::vector<std::string> expected = {"David", "Emily", "Justin"};
    EXPECT_EQ(actual, expected);
    //Empty case
    actual = StringUtils::Split("", ",");
    expected = {};
    EXPECT_EQ(actual, expected);
    //Case with no split
    actual = StringUtils::Split("Justin", ",");
    expected = {"Justin"};
    EXPECT_EQ(actual, expected);
    //Case with split at the start
    actual = StringUtils::Split(",Justin,David,Emily", ",");
    expected = {"","Justin", "David", "Emily"};
    EXPECT_EQ(actual, expected);
}


TEST(StringUtilsTest, Join){
    EXPECT_EQ(StringUtils::Join("-", {"Justin","David","Emily"}),"Justin-David-Emily");
    EXPECT_EQ(StringUtils::Join("-", {}),"");
    EXPECT_EQ(StringUtils::Join("-", {"Justin"}),"Justin");
    EXPECT_EQ(StringUtils::Join("-", {"","David",""}),"-David-");
}

TEST(StringUtilsTest, ExpandTabs){
    std::string Base = "Hello\tWorld";
    EXPECT_EQ(StringUtils::ExpandTabs(Base, 4), "Hello   World");
    EXPECT_EQ(StringUtils::ExpandTabs(Base, 8), "Hello   World");
    EXPECT_EQ(StringUtils::ExpandTabs(Base, 2), "Hello World");
    Base = "Ju\tstin Ch\to";
    EXPECT_EQ(StringUtils::ExpandTabs(Base,3), "Ju stin Ch  o");
    EXPECT_EQ(StringUtils::ExpandTabs(Base,7), "Ju     stin Ch       o");
    EXPECT_EQ(StringUtils::ExpandTabs(Base,4), "Ju  stin Ch o");
}

TEST(StringUtilsTest, EditDistance){
       //Check Case sensitivity requirement
       EXPECT_EQ(StringUtils::EditDistance("Hello", "hello", true), 0);
       EXPECT_EQ(StringUtils::EditDistance("Hello", "hello", false), 1);
       //Empty String requirement
       EXPECT_EQ(StringUtils::EditDistance("", "", true), 0);
       EXPECT_EQ(StringUtils::EditDistance("Hello", "", true), 5);
       EXPECT_EQ(StringUtils::EditDistance("", "Hello", true), 5);
       EXPECT_EQ(StringUtils::EditDistance("Hello", "Hello", true), 0);
       EXPECT_EQ(StringUtils::EditDistance("abcde", "fghij", true), 5);
       //Test different lengths
       EXPECT_EQ(StringUtils::EditDistance("Justin", "kmrewplaq", true), 9);

    
}
