#include <gtest/gtest.h>
#include "../../src/application/use_cases/RegisterUser.h"
#include "../../src/application/dto/RegisterUserRequest.h"
#include "../mocks/MockUserRepository.h"

TEST(RegisterUserTests, RegistersNewUser) {
    MockUserRepository repo;
    application::use_cases::RegisterUser useCase(repo);

    application::dto::RegisterUserRequest req{"Lin", "lin@example.com"};
    auto result = useCase.execute(req);

    EXPECT_TRUE(repo.storedUser.has_value());
    EXPECT_EQ(repo.storedUser->name(), "Lin");
    EXPECT_EQ(repo.storedUser->email().value(), "lin@example.com");
}

TEST(RegisterUserTests, RejectsDuplicateUser) {
    MockUserRepository repo;
    application::use_cases::RegisterUser useCase(repo);

    // First registration
    application::dto::RegisterUserRequest req1{"Lin", "lin@example.com"};
    useCase.execute(req1);

    // Duplicate
    application::dto::RegisterUserRequest req2{"Lin", "lin@example.com"};
    auto result = useCase.execute(req2);

    EXPECT_EQ(result, "User already exists");
}
