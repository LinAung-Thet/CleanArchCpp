#include <gtest/gtest.h>
#include "../../src/domain/value_objects/Email.h"

TEST(EmailTests, ValidEmailAccepted) {
    domain::value_objects::Email email("user@example.com");
    EXPECT_EQ(email.value(), "user@example.com");
}

TEST(EmailTests, InvalidEmailThrows) {
    EXPECT_THROW(domain::value_objects::Email("invalid-email"), std::invalid_argument);
}