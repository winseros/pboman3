#include <gtest/gtest.h>
#include "domain/documentheader.h"
#include "domain/validationexception.h"

namespace pboman3::domain::test {
    TEST(DocumentHeaderTest, Ctor_Initializes_Object) {
        const DocumentHeader header1("h1", "v1");
        ASSERT_EQ(header1.name(), "h1");
        ASSERT_EQ(header1.value(), "v1");

        const DocumentHeader header2("h2", "");
        ASSERT_EQ(header2.name(), "h2");
        ASSERT_EQ(header2.value(), "");
    }

    TEST(DocumentHeaderTest, Ctor_Throws_If_Name_Empty) {
        ASSERT_THROW(DocumentHeader header("", "v1"), ValidationException);
    }

    TEST(DocumentHeaderTest, Repository_Does_Not_Throw) {
        const DocumentHeader header1(DocumentHeader::InternalData{ "h1", "v1" });
        ASSERT_EQ(header1.name(), "h1");
        ASSERT_EQ(header1.value(), "v1");

        const DocumentHeader header2(DocumentHeader::InternalData{ "", "" });
        ASSERT_EQ(header2.name(), "");
        ASSERT_EQ(header2.value(), "");
    }

    TEST(DocumentHeaderTest, Equality_Operator_Returns_True) {
        const DocumentHeader header1(DocumentHeader::InternalData{ "h1", "v1" });
        const DocumentHeader header2(DocumentHeader::InternalData{ "h1", "v1" });

        ASSERT_EQ(header1, header2);
    }

    TEST(DocumentHeaderTest, Equality_Operator_Returns_False) {
        const DocumentHeader header1(DocumentHeader::InternalData{ "h1", "v2" });
        const DocumentHeader header2(DocumentHeader::InternalData{ "h1", "v1" });

        ASSERT_NE(header1, header2);

        const DocumentHeader header3(DocumentHeader::InternalData{ "h2", "v1" });
        const DocumentHeader header4(DocumentHeader::InternalData{ "h1", "v1" });

        ASSERT_NE(header3, header4);
    }
}
