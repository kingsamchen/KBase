/*
 @ 0xCCCCCCCC
*/

#include "stdafx.h"

#include "gtest/gtest.h"
#include "kbase/string_view.h"

TEST(StringViewTest, Ctor)
{
	// for default constructor
	{
		kbase::StringView empty_view;
		EXPECT_TRUE(empty_view.empty());
		EXPECT_EQ(nullptr, empty_view.data());
		EXPECT_EQ(0, empty_view.length());
	}

	// for BasicStringView(const CharT* str, size_type count)
	{
		const char* s = "hello, world";
		size_t len = strlen(s);
		kbase::StringView s_v(s, len);
		EXPECT_FALSE(s_v.empty());
		std::cout << s_v.data() << std::endl << s_v.length() << std::endl;
		EXPECT_EQ(s, s_v.data());
		EXPECT_EQ(len, s_v.length());

		std::string str = "this is a test text";
		kbase::StringView s_v2(str.data(), str.length());
		EXPECT_FALSE(s_v2.empty());
		std::cout << s_v2.data() << std::endl << s_v2.length() << std::endl;
		EXPECT_EQ(str.data(), s_v2.data());
		EXPECT_EQ(str.length(), s_v2.length());
	}

	// for copy constructor
	{
		std::string str = "this is a test text";
		kbase::StringView s_v(str.data(), str.length());
		kbase::StringView s_v_copy(s_v);
		EXPECT_TRUE(s_v.data() == s_v_copy.data());
		EXPECT_TRUE(s_v.length() == s_v_copy.length());
	}

	// for BasicStringView(const CharT* str)
	{
		const char* s = "hello, world";
		size_t len = strlen(s);
		kbase::StringView s_v(s);
		EXPECT_EQ(s, s_v.data());
		EXPECT_EQ(len, s_v.length());
	}

	// for BasicStringView(const std::basic_string<CharT>& str)
	{
		std::string str = "this is a test text";
		kbase::StringView s_v(str);
		EXPECT_EQ(str.data(), s_v.data());
		EXPECT_EQ(str.length(), s_v.length());
	}
}

TEST(StringViewTest, AssignOperator)
{
	kbase::StringView v = "Hello, world";
	kbase::StringView v_ex = "this is a test text";
	EXPECT_FALSE(v.length() == v_ex.length());
	v = v_ex;
	EXPECT_TRUE(v.length() == v_ex.length());
}

TEST(StringViewTest, MaxSize)
{
	kbase::StringView view;
	auto max_size = std::numeric_limits<size_t>::max();
	EXPECT_EQ(max_size, view.max_size());
}

TEST(StringViewTest, Iterator)
{
	const char* raw_str = "hello world";
	kbase::StringView view = raw_str;
	std::string str(view.begin(), view.end());
	std::cout << str << std::endl;
	EXPECT_EQ(std::string(raw_str), str);

	std::string rstr(view.rbegin(), view.rend());
	std::cout << rstr << std::endl;
	std::reverse(str.begin(), str.end());
	EXPECT_EQ(str, rstr);
}

TEST(StringViewTest, ElementAccess)
{
	kbase::StringView view = "hello world";

	// for operator[]
	EXPECT_EQ('h', view[0]);
	EXPECT_EQ('d', view[view.length() - 1]);

	// for at
	EXPECT_EQ('h', view.at(0));
	EXPECT_EQ('d', view.at(view.length() - 1));
	EXPECT_THROW(view.at(view.size()), std::out_of_range);

	// for front
	EXPECT_EQ('h', view.front());

	// for back
	EXPECT_EQ('d', view.back());
}

TEST(StringViewTest, Modifiers)
{
	kbase::StringView view = "hello world";
	size_t space_pos = std::distance(view.begin(), std::find(view.begin(), view.end(), ' '));
	std::cout << space_pos << std::endl;
	ASSERT_TRUE(space_pos <= view.length());

	auto v_prefix = view;
	v_prefix.RemovePrefix(space_pos + 1);
	EXPECT_EQ(std::string("world"), std::string(v_prefix.data(), v_prefix.length()));

	auto v_suffix = view;
	v_suffix.RemoveSuffix(view.length() - space_pos);
	EXPECT_EQ(std::string("hello"), std::string(v_suffix.data(), v_suffix.length()));

	kbase::StringView v_1 = "hello";
	kbase::StringView v_2 = "world";
	v_1.swap(v_2);
	EXPECT_EQ(std::string("world"), std::string(v_1.data(), v_1.length()));
	EXPECT_EQ(std::string("hello"), std::string(v_2.data(), v_2.length()));
}

TEST(StringViewTest, ToStdString)
{
	std::string str = "hello, world";
	kbase::StringView view = str;
	EXPECT_EQ(str, view.ToString());

	kbase::StringView empty_view;
	EXPECT_EQ(std::string(), empty_view.ToString());
}

TEST(StringViewTest, Copy)
{
	kbase::StringView view = "hello, world";
	std::vector<char> buf(32);

	{
		size_t count_copied = view.copy(buf.data(), 5);
		EXPECT_EQ(5, count_copied);
		EXPECT_STREQ("hello", buf.data());
	}

	buf.clear(); buf.resize(32);
	{
		size_t count_copied = view.copy(buf.data(), 10, 7);
		EXPECT_NE(10, count_copied);
		EXPECT_STREQ("world", buf.data());
	}

	buf.clear(); buf.resize(32);
	{
		EXPECT_THROW(view.copy(buf.data(), 5, view.length() + 2), std::out_of_range);
	}
}

TEST(StringViewTest, Substr)
{
	kbase::StringView view = "hello, world";

	EXPECT_EQ(std::string(", world"), view.substr(5).ToString());
	EXPECT_EQ(std::string("hello"), view.substr(0, 5).ToString());
	EXPECT_THROW(view.substr(view.length() + 2), std::out_of_range);
}

TEST(StringViewTest, Compare)
{
	kbase::StringView v_1 = "hello";
	kbase::StringView v_2 = "hell";
	EXPECT_TRUE(v_1.compare(v_2) > 0);

	EXPECT_TRUE(v_1.compare(0, 4, v_2) == 0);

	EXPECT_TRUE(v_1.compare(2, 2, v_2, 2, 2) == 0);

	EXPECT_TRUE(v_1.compare("he") > 0);

	EXPECT_TRUE(v_1.compare(0, 2, "he") == 0);

	EXPECT_TRUE(v_1.compare(0, 2, "hell", 2) == 0);
}

TEST(StringViewTest, Find)
{
	kbase::StringView view = "hello, world";
	kbase::StringView view_ex = "where there is a will, there is a way";
	kbase::StringView view_ng = "This is a string";
	auto npos = decltype(view)::npos;

	// for find
	EXPECT_EQ(0, view.find("hell"));
	EXPECT_EQ(8, view.find("or"));
	EXPECT_EQ(8, view.find("or", 7));
	EXPECT_EQ(npos, view.find("joke"));
	EXPECT_EQ(npos, view.find("world", 10));
	EXPECT_EQ(4, view.find('o'));
	EXPECT_EQ(0, view.find("here", 0, 2));

	// for rfind
	EXPECT_EQ(10, view.rfind('l'));
	EXPECT_EQ(npos, view.rfind('k'));
	EXPECT_EQ(23, view_ex.rfind("there"));
	EXPECT_EQ(17, view_ex.rfind("will", 22));
	EXPECT_EQ(5, view_ng.rfind("is"));
	EXPECT_EQ(2, view_ng.rfind("is", 4));
}

TEST(StringViewTest, FindOf)
{
	kbase::StringView view = "where there is a will, there is a way";
	auto npos = decltype(view)::npos;

	// for find_first_of
	EXPECT_EQ(5, view.find_first_of(" ,"));
	EXPECT_EQ(2, view.find_first_of("aeiou"));

	// for find_last_of
	kbase::StringView view_1 = "abcd-1234-abcd-1234";
	EXPECT_EQ(13, view_1.find_last_of("d", 14));
	EXPECT_EQ(npos, view_1.find_last_of("x"));

	kbase::StringView view_2 = "ABCD-1234-ABCD-1234";
	EXPECT_EQ(11, view_2.find_last_of("B1", 12));
	EXPECT_EQ(16, view_2.find_last_of("D2"));

	kbase::StringView view_3 = "456-EFG-456-EFG";
	EXPECT_EQ(4, view_3.find_last_of("5E", 8));

	kbase::StringView view_4 = "12-ab-12-ab";
	EXPECT_EQ(4, view_4.find_last_of("ba3", 8));
	EXPECT_EQ(9, view_4.find_last_of("a2"));
}

TEST(StringViewTest, FindNotOf)
{
	{
		kbase::StringView view_1 = "xddd-1234-abcd";
		EXPECT_EQ(4, view_1.find_first_not_of("d", 2));
		EXPECT_EQ(1, view_1.find_first_not_of("x"));

		kbase::StringView view_2 = "BBB-1111";
		EXPECT_EQ(kbase::StringView::npos, view_2.find_first_not_of("B1", 6));
		EXPECT_EQ(3, view_2.find_first_not_of("B2"));

		kbase::StringView view_3 = "444-555-GGG";
		EXPECT_EQ(3, view_3.find_first_not_of("45G"));

		kbase::StringView view_4 = "12-ab-12-ab";
		EXPECT_EQ(5, view_4.find_first_not_of("ba3", 5));
		EXPECT_EQ(2, view_4.find_first_not_of("12"));
	}

	{
		kbase::StringView view_1 = "dddd-1dd4-abdd";
		EXPECT_EQ(5, view_1.find_last_not_of("d", 7));
		EXPECT_EQ(11, view_1.find_last_not_of("d"));

		kbase::StringView view_2 = "BBB-1111";
		EXPECT_EQ(3, view_2.find_last_not_of("B1", 6));
		EXPECT_EQ(kbase::StringView::npos, view_2.find_last_not_of("B-1"));

		kbase::StringView view_3 = "444-555-GGG";
		EXPECT_EQ(7, view_3.find_last_not_of("45G"));
		EXPECT_EQ(3, view_3.find_last_not_of("45G", 6, 6));

		kbase::StringView view_4 = "12-ab-12-ab";
		EXPECT_EQ(1, view_4.find_last_not_of("b-a", 5));
		EXPECT_EQ(10, view_4.find_last_not_of("12"));
	}
}

TEST(StringViewTest, OperatorComparison)
{
	kbase::StringView view_1 = "abc";
	kbase::StringView view_2 = "adc";
	EXPECT_FALSE(view_1 == view_2);
	EXPECT_TRUE(view_1 != view_2);
}

TEST(StringViewTest, Output)
{
	kbase::StringView v = "hello world";
	kbase::WStringView w = L"test text";
	std::cout << v << std::endl;
	std::wcout << w << std::endl;
}

TEST(StringViewTest, Hash)
{
	kbase::StringView v = "hello world";
	kbase::WStringView w = L"hello world";
	EXPECT_EQ(std::hash<std::string>()(v.ToString()), std::hash<decltype(v)>()(v));
	EXPECT_EQ(std::hash<std::wstring>()(w.ToString()), std::hash<decltype(w)>()(w));
}