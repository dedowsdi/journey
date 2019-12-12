#include <string>
#include <algorithm>
#include <iostream>
#include <set>
#include <vector>

auto collectSubStrings(
    const std::string& s, std::string::size_type maxSubLength )
{
    auto l = s.length();
    auto res = std::set<std::string>();
    for ( auto start = 0u; start < l; start++ )
    {
        auto m =
            std::min<std::string::size_type>( maxSubLength, l - start + 1u );
        for ( auto length = 1u; length < m; length++ )
        {
            res.insert( s.substr( start, length ) );
        }
    }
    return res;
}

// longest common substring
std::string longestCommonSubstring( const std::string& s0, const std::string& s1 )
{
    // collect substring set
    auto maxSubLength = std::min( s0.length(), s1.length() );
    auto set0 = collectSubStrings( s0, maxSubLength );
    auto set1 = collectSubStrings( s1, maxSubLength );

    // get commons into a vector
    auto common = std::vector<std::string>();
    std::set_intersection( set0.begin(), set0.end(), set1.begin(), set1.end(),
        std::back_inserter( common ) );

    // get the longest one
    std::nth_element( common.begin(), common.begin(), common.end(),
        []( const std::string& s1, const std::string& s2 ) {
            return s1.length() > s2.length();
        } );
    return *common.begin();
}
 
int main( int argc, char* argv[] )
{
    auto s1 = std::string( "thisisatest" );
    auto s2 = std::string( "testing123testing" );
    std::cout << "The longest common substring of " << s1 << " and " << s2
              << " is:\n";
    std::cout << "\"" << longestCommonSubstring( s1, s2 ) << "\" !\n";
    return 0;
}
