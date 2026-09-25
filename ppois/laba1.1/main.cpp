//
// Created by Ivan Triputko on 25.09.2026.
//

#include <UnitTest++/UnitTest++.h>
#include "set.h"

SUITE(SetTests) {

    TEST(BasicOperations) {
        Set s;
        CHECK(s.isEmpty());
        CHECK_EQUAL(0, s.getSize());
        s.addElement("a");
        s.addElement("b");
        CHECK(!s.isEmpty());
        CHECK_EQUAL(2, s.getSize());
        s.addElement("a");
        CHECK_EQUAL(2, s.getSize());
        Set sub;
        sub.addElement("x");
        s.addElement(sub);
        CHECK_EQUAL(3, s.getSize());
        s.deleteElement("a");
        CHECK_EQUAL(2, s.getSize());
        CHECK(!s["a"]);
        s.deleteElement(sub);
        CHECK_EQUAL(1, s.getSize());
        CHECK(!s[sub]);
    }

    TEST(ParseStringStringOverload) {
        Set s;
        std::string input = "{a, b, {c}}";
        s.parseString(input);
        CHECK_EQUAL(3, s.getSize());
        CHECK(s["a"]);
        CHECK(s["b"]);
        Set expected_sub;
        expected_sub.addElement("c");
        CHECK(s[expected_sub]);
    }

    TEST(ParseStringCStringOverload) {
        Set s;
        const char* input = "{a, b, {c}}";
        s.parseString(input);
        CHECK_EQUAL(3, s.getSize());
        CHECK(s["a"]);
        CHECK(s["b"]);
        Set expected_sub;
        expected_sub.addElement("c");
        CHECK(s[expected_sub]);
    }

    TEST(ParseStringEquivalence) {
        Set s1, s2;
        s1.parseString(std::string("{1, 2, {3}}"));
        s2.parseString("{1, 2, {3}}");
        CHECK(s1 == s2);
    }

    TEST(OperatorEqual) {
        Set s1, s2;
        s1.addElement("x").addElement("y");
        s2.addElement("x").addElement("y");
        CHECK(s1 == s2);
        s2.addElement("z");
        CHECK(!(s1 == s2));
    }

    TEST(OperatorContains) {
        Set s;
        s.addElement("alpha");
        Set sub;
        sub.addElement("beta");
        s.addElement(sub);
        CHECK(s["alpha"]);
        CHECK(!s["gamma"]);
        CHECK(s[sub]);
    }

    TEST(OperatorUnion) {
        Set s1, s2;
        s1.addElement("a").addElement("b");
        s2.addElement("b").addElement("c");
        Set result = s1 + s2;
        CHECK_EQUAL(3, result.getSize());
        CHECK(result["a"]);
        CHECK(result["b"]);
        CHECK(result["c"]);
        s1 += s2;
        CHECK(s1 == result);
    }

    TEST(OperatorDifference) {
        Set s1, s2;
        s1.addElement("a").addElement("b").addElement("c");
        s2.addElement("b");
        Set result = s1 - s2;
        CHECK_EQUAL(2, result.getSize());
        CHECK(result["a"]);
        CHECK(!result["b"]);
        CHECK(result["c"]);
        s1 -= s2;
        CHECK(s1 == result);
    }

    TEST(OperatorIntersection) {
        Set s1, s2;
        s1.addElement("a").addElement("b");
        s2.addElement("b").addElement("c");
        Set result = s1 * s2;
        CHECK_EQUAL(1, result.getSize());
        CHECK(result["b"]);
        CHECK(!result["a"]);
        s1 *= s2;
        CHECK(s1 == result);
    }

    TEST(PowerSetEmpty) {
        Set s;
        Set power = s.powerSet();
        CHECK_EQUAL(1, power.getSize());
    }

    TEST(PowerSetSizeVerification) {
        Set s;
        s.addElement("a");
        s.addElement("b");
        Set sub;
        sub.addElement("c");
        s.addElement(sub);
        Set power = s.powerSet();
        CHECK_EQUAL(8, power.getSize());
    }

    TEST(PowerSetContent) {
        Set s;
        s.addElement("a");
        s.addElement("b");
        Set power = s.powerSet();
        Set emptySet;
        Set setA; setA.addElement("a");
        Set setB; setB.addElement("b");
        Set setAB; setAB.addElement("a").addElement("b");
        CHECK(power[emptySet]);
        CHECK(power[setA]);
        CHECK(power[setB]);
        CHECK(power[setAB]);
    }
}

int main() {
    return UnitTest::RunAllTests();
}