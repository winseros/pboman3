#include <gtest/gtest.h>
#include "model/signaturemodel.h"

namespace pboman3::test {
    TEST(SignatureModelTest, SignatureBytes_Returns_Empty_By_Default) {
        const SignatureModel m;
        ASSERT_TRUE(m.signatureString().isNull());
    }

    TEST(SignatureModelTest, SetSignatureBytes_Sets_Bytes) {
        SignatureModel m;

        m.setSignatureBytes(QByteArray(10, 5));
        ASSERT_EQ(m.signatureString(), "05 05 05 05 05 05 05 05 05 05");

        m.setSignatureBytes(QByteArray());
        ASSERT_TRUE(m.signatureString().isNull());
    }
}
