#include "stdafx.h"
#include "regex_helpers.h"

#include <regex>
#include <string>
#include <vector>
#include <map>
#include <functional>

namespace regex_helpers
{
	#define DEFAULTNUMBERFORMAT "(\\#?\\-?[$|%]??[0-9A-Fa-f]+)"
	#define DEFAULTPARENOPERATORS "[\\+|\\-|\\*|x|\\/]"

	// SYMBOLS
	// ----

	using stdstringintmap = std::map<std::string, int>;
	using mfcstringvector = std::vector<CString>;
	using stdstringvector = std::vector<std::string>;
	using OperationFunc = std::function<long(long left, long right)>;

	const static auto rxnamedvalue = std::regex("([A-Za-z_][0-9A-Za-z_]+)");


	// DECLARATIONS
	// ----

	std::string build_infix_numformat(std::string numberformat, std::string operations);
	std::string build_paren_numformat(std::string numberformat, std::string operations);

	long add(long left, long right);
	long subtract(long left, long right);
	long multiply(long left, long right);
	long divide(long left, long right);

	long convert_number(std::string strnumber);

	std::string remove_doublenegatives(std::string originaltext);
	std::string remove_matches(std::string originaltext, std::regex rxpattern);
	std::string replace_values(std::string originaltext, const stdstringintmap & valuemap, std::regex rxpattern);
	std::string eval_expression(std::string originaltext, std::regex rxpattern);
	std::string resolve_parens(std::string originaltext, std::regex rxpattern);
	std::string remove_matches(std::string originaltext, std::regex rxnumberformat);
	long evaluate_infix(std::string formula, std::string numberformatrxpattern);


	// EXTERNAL IMPLEMENTATION
	// ----

	long eval_formula(std::string formula, const stdstringintmap & valuemap)
	{
		return eval_formula(formula, valuemap, DEFAULTNUMBERFORMAT);
	}

	long eval_formula(std::string formula, const stdstringintmap & valuemap, std::string numberformatrxpattern)
	{
		// Step 1: replace the values
		auto newformula = replace_values(formula, valuemap, rxnamedvalue);

		// Step 2: replace the paren expressions
		auto rxparens = std::regex(build_paren_numformat(numberformatrxpattern, DEFAULTPARENOPERATORS));
		newformula = resolve_parens(newformula, rxparens);

		// Step 4: evaluate the formula as infix
		auto value = evaluate_infix(newformula, numberformatrxpattern);
		return value;
	}


	// INTERNAL IMPLEMENTATION
	// ----

	std::string build_infix_numformat(std::string numberformat, std::string operations)
	{
		return numberformat + "\\s*(" + operations + ")\\s*" + numberformat;
	}
	std::string build_paren_numformat(std::string numberformat, std::string operations)
	{
		return "\\(\\s*" + numberformat + "\\s*(" + operations + ")?\\s*" + numberformat + "?\\s*\\)";
	}

	long add(long left, long right) { return left + right; }

	long subtract(long left, long right) { return left - right; }

	long multiply(long left, long right) { return left * right; }

	long divide(long left, long right) {
		if (right == 0) throw std::exception("Operaton / threw a divied by zero exception");
		return left / right;
	}

	long convert_number(std::string strnumber)
	{
		// strip leading literal character if present
		if (!strnumber.empty() && strnumber[0] == '#') strnumber.erase(0, 1);

		int base = 10;
		if (strnumber.empty()) base = 0;
		auto mark = strnumber.find_first_of("$%");
		if (mark != std::string::npos) {
			if (strnumber[mark] == '$') base = 16;
			else base = 2;
			strnumber.erase(mark, 1);
		}
		return std::stol(strnumber, nullptr, base);
	}

	std::string remove_doublenegatives(std::string originaltext)
	{
		const static auto rxdoublenegs = std::regex("\\-\\-");
		return remove_matches(originaltext, rxdoublenegs);
	}

	std::string remove_matches(std::string originaltext, std::regex rxpattern)
	{
		auto newtext = std::regex_replace(originaltext, rxpattern, "");
		return newtext;
	}

	std::string replace_values(std::string originaltext, const stdstringintmap & valuemap, std::regex rxpattern)
	{
		std::string newtext;
		auto rxend = std::sregex_iterator();
		auto rxbegin = std::sregex_iterator(cbegin(originaltext), cend(originaltext), rxpattern);
		if (rxbegin == rxend)
			return originaltext;

		auto rxlast = std::sregex_iterator();
		for (auto i = rxbegin; i != rxend; rxlast = i, ++i) {
			auto name = i->str();
			//DEVNOTE - workaround, try to detect $addr values and just emit them as is.
			//REFACTOR - trap 'key not found' error and throw either std::exception or a custom exception here?
			auto prefix = i->prefix().str();
			auto mappedvalue = (prefix.find('$') != std::string::npos) ? name : std::to_string(valuemap.find(name)->second);
			newtext += i->prefix().str();
			newtext += mappedvalue;
		}
		if (rxlast != rxend) newtext += rxlast->suffix();
		return newtext;
	}

	std::string eval_expression(std::string originaltext, std::regex rxpattern)
	{
		static auto operationmap = std::map<std::string, OperationFunc>{
			{ "+", add },
			{ "-", subtract },
			{ "*", multiply },
			{ "x", multiply },
			{ "/", divide },
		};

		if (originaltext.empty()) return originaltext;

		std::string text = remove_doublenegatives(originaltext);

		std::smatch match;
		if (!std::regex_match(text, match, rxpattern))
			return text;

		if (match.size() != 4) {
			std::string message = "parens match found " + std::to_string(match.size()) + " elements instead of 3 elements (value, operator, and value)";
			throw std::exception(message.c_str());
		}

		if (!match[2].matched && !match[3].matched) {
			// A single parenthesized term, like (10)
			auto term = match[1].str();
			auto result = std::to_string(convert_number(term));
			return result;
		}

		// A parenthesized operation, like (10 * 7) or (5 + -3)
		auto left = convert_number(match[1].str());
		auto right = convert_number(match[3].str());
		auto op = match[2].str();
		//REFACTOR - catch the 'key not found' exception and throw a custom exception here?
		auto func = operationmap[op];
		auto result = std::to_string(func(left, right));
		return result;
	}

	std::string resolve_parens(std::string originaltext, std::regex rxpattern)
	{
		std::string finaltext = originaltext;
		auto rxend = std::sregex_iterator();

		auto rxbegin = std::sregex_iterator(cbegin(finaltext), cend(finaltext), rxpattern);
		while (rxbegin != rxend) {
			std::string newtext;
			auto rxlast = std::sregex_iterator();
			for (auto i = rxbegin; i != rxend; rxlast = i, ++i) {
				//REFACTOR - trap 'bad operator' errors and throw std::exception or a custom exception here?
				newtext += i->prefix().str();
				newtext += eval_expression(i->str(), rxpattern);
			}
			if (rxlast != rxend) newtext += rxlast->suffix();
			finaltext = remove_doublenegatives(newtext);
			rxbegin = std::sregex_iterator(cbegin(finaltext), cend(finaltext), rxpattern);
		}

		return finaltext;
	}

	long evaluate_infix(std::string formula, std::string numberformatrxpattern)
	{
		std::string finaltext = formula;
		auto rxend = std::sregex_iterator();

		// Each character is an operator. Each string means operators of equal left-right precedence
		auto operators = std::vector<std::string>{ "\\*|x|\\/", "\\+|\\-" };
		for (auto ops : operators) {
			auto rxinfix = std::regex(build_infix_numformat(numberformatrxpattern, ops));
			auto rxbegin = std::sregex_iterator(cbegin(finaltext), cend(finaltext), rxinfix);
			while (rxbegin != rxend) {
				std::string newtext;
				auto rxlast = std::sregex_iterator();
				for (auto i = rxbegin; i != rxend; rxlast = i, ++i) {
					//REFACTOR - trap 'bad operator' errors a throw std::exception or a custom exception here?
					newtext += i->prefix().str();
					newtext += eval_expression(i->str(), rxinfix);
				}

				if (rxlast != rxend) newtext += rxlast->suffix();
				finaltext = newtext;
				rxbegin = std::sregex_iterator(cbegin(finaltext), cend(finaltext), rxinfix);
			}
		}

		return convert_number(finaltext);
	}

}