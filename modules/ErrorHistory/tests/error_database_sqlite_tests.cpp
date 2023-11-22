// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <catch2/catch_all.hpp>

#include "../ErrorDatabaseSqlite.hpp"
#include "helpers.hpp"

SCENARIO("Check ErrorDatabaseSqlite class", "[!throws]") {
    GIVEN("An ErrorDatabaseSqlite object") {
        const std::string bin_dir = get_bin_dir().string() + "/";
        const std::string db_name = get_unique_db_name();
        TestDatabase db(bin_dir + "/databases/" + db_name, true);
        WHEN("Getting all errors") {
            THEN("The database should be empty") {
                auto errors = db.get_errors(std::list<Everest::error::ErrorFilter>());
                REQUIRE(errors.empty());
            }
        }
        WHEN("Adding an error") {
            std::vector<Everest::error::ErrorPtr> test_errors = {std::make_shared<Everest::error::Error>(
                "test_type", "test_message", "test_description",
                ImplementationIdentifier("test_from_module", "test_from_implementation"), Everest::error::Severity::Low,
                date::utc_clock::now(), Everest::error::UUID(), Everest::error::State::Active)};
            db.add_error(test_errors.at(0));
            THEN("The error should be in the database") {
                check_expected_errors_in_list(test_errors, db.get_errors(std::list<Everest::error::ErrorFilter>()));
            }
        }
        WHEN("Adding multiple errors") {
            std::vector<Everest::error::ErrorPtr> test_errors = {
                std::make_shared<Everest::error::Error>(
                    "test_type_a", "test_message_a", "test_description_a",
                    ImplementationIdentifier("test_from_module_a", "test_from_implementation_a"),
                    Everest::error::Severity::High, date::utc_clock::now(), Everest::error::UUID(),
                    Everest::error::State::ClearedByModule),
                std::make_shared<Everest::error::Error>(
                    "test_type_b", "test_message_b", "test_description_b",
                    ImplementationIdentifier("test_from_module_b", "test_from_implementation_b"),
                    Everest::error::Severity::Medium, date::utc_clock::now(), Everest::error::UUID(),
                    Everest::error::State::ClearedByReboot)};
            for (Everest::error::ErrorPtr error : test_errors) {
                db.add_error(error);
            }
            THEN("The errors should be in the database") {
                auto errors = db.get_errors(std::list<Everest::error::ErrorFilter>());
                check_expected_errors_in_list(test_errors, errors);
            }
        }
    }
    GIVEN("12 Errors in a connected ErrorDatabaseSqlite object") {
        const std::string bin_dir = get_bin_dir().string() + "/";
        const std::string db_name = get_unique_db_name();
        TestDatabase db(bin_dir + "/databases/" + db_name, true);
        std::vector<Everest::error::ErrorPtr> test_errors = get_test_errors();
        for (Everest::error::ErrorPtr error : test_errors) {
            db.add_error(error);
        }
        WHEN("Getting all errors") {
            auto errors = db.get_errors(std::list<Everest::error::ErrorFilter>());
            THEN("The result should contain all 12 errors") {
                check_expected_errors_in_list(test_errors, errors);
            }
        }
        WHEN("Getting all errors with StateFilter") {
            auto errors = db.get_errors({Everest::error::ErrorFilter(Everest::error::StateFilter::Active)});
            THEN("The result should contain specific errors") {
                std::vector<Everest::error::ErrorPtr> expected_errors({test_errors[3], test_errors[4], test_errors[5]});
                check_expected_errors_in_list(expected_errors, errors);
            }
        }
        WHEN("Getting all errors with OriginFilter") {
            auto errors = db.get_errors({Everest::error::ErrorFilter(
                Everest::error::OriginFilter("test_from_module_a", "test_from_implementation_a"))});
            THEN("The result should contain specific errors") {
                std::vector<Everest::error::ErrorPtr> expected_errors(
                    {test_errors[0], test_errors[2], test_errors[4], test_errors[6], test_errors[8], test_errors[10]});
                check_expected_errors_in_list(expected_errors, errors);
            }
        }
        WHEN("Getting all errors with TypeFilter") {
            auto errors = db.get_errors({Everest::error::ErrorFilter(Everest::error::TypeFilter("test_type_c"))});
            THEN("The result should contain specific errors") {
                std::vector<Everest::error::ErrorPtr> expected_errors(
                    {test_errors[2], test_errors[3], test_errors[4], test_errors[5], test_errors[9], test_errors[10]});
                check_expected_errors_in_list(expected_errors, errors);
            }
        }
        WHEN("Getting all errors with SeverityFilter") {
            auto errors = db.get_errors({Everest::error::ErrorFilter(Everest::error::SeverityFilter::MEDIUM_GE)});
            THEN("The result should contain specific errors") {
                std::vector<Everest::error::ErrorPtr> expected_errors({test_errors[4], test_errors[5], test_errors[6],
                                                                       test_errors[7], test_errors[8], test_errors[9],
                                                                       test_errors[10], test_errors[11]});
                check_expected_errors_in_list(expected_errors, errors);
            }
        }
        WHEN("Getting all errors with TimePeriodFilter") {
            auto errors = db.get_errors({Everest::error::ErrorFilter(
                Everest::error::TimePeriodFilter{date::utc_clock::now() + std::chrono::minutes(150),
                                                 date::utc_clock::now() + std::chrono::minutes(270)})});
            THEN("The result should contain specific errors") {
                std::vector<Everest::error::ErrorPtr> expected_errors({test_errors[3], test_errors[4]});
                check_expected_errors_in_list(expected_errors, errors);
            }
        }
        WHEN("Getting all errors with HandleFilter") {
            auto errors =
                db.get_errors({Everest::error::ErrorFilter(Everest::error::HandleFilter(test_errors[4]->uuid))});
            THEN("The result should contain specific errors") {
                std::vector<Everest::error::ErrorPtr> expected_errors({test_errors[4]});
                check_expected_errors_in_list(expected_errors, errors);
            }
        }
        WHEN("Getting all errors with multiple filters") {
            auto errors = db.get_errors({Everest::error::ErrorFilter(Everest::error::StateFilter::Active),
                                         Everest::error::ErrorFilter(Everest::error::OriginFilter(
                                             "test_from_module_a", "test_from_implementation_a"))});
            THEN("The result should contain specific errors") {
                std::vector<Everest::error::ErrorPtr> expected_errors({test_errors[4]});
                check_expected_errors_in_list(expected_errors, errors);
            }
        }
        WHEN("Filtering all errors out") {
            auto errors = db.get_errors({
                Everest::error::ErrorFilter(Everest::error::StateFilter::ClearedByModule),
                Everest::error::ErrorFilter(
                    Everest::error::OriginFilter("test_from_module_a", "test_from_implementation_a")),
                Everest::error::ErrorFilter(Everest::error::TypeFilter("test_type_c")),
                Everest::error::ErrorFilter(Everest::error::SeverityFilter::HIGH_GE),
            });
            THEN("The result should contain no errors") {
                REQUIRE(errors.empty());
            }
        }

        WHEN("Edit error type") {
            std::list<Everest::error::ErrorFilter> filters = {
                Everest::error::ErrorFilter(Everest::error::HandleFilter(test_errors[4]->uuid))};
            Everest::error::ErrorDatabase::EditErrorFunc edit_func = [](Everest::error::ErrorPtr error) {
                error->type = "new_type";
            };
            REQUIRE(db.get_errors(filters).size() > 0);
            db.edit_errors(filters, edit_func);
            THEN("The error should be edited") {
                auto errors = db.get_errors(filters);
                REQUIRE(errors.size() == 1);
                REQUIRE(errors.front()->type == "new_type");
            }
        }
        WHEN("Edit error state") {
            std::list<Everest::error::ErrorFilter> filters = {
                Everest::error::ErrorFilter(Everest::error::HandleFilter(test_errors[4]->uuid))};
            Everest::error::ErrorDatabase::EditErrorFunc edit_func = [](Everest::error::ErrorPtr error) {
                error->state = Everest::error::State::ClearedByModule;
            };
            REQUIRE(db.get_errors(filters).size() > 0);
            db.edit_errors(filters, edit_func);
            THEN("The error should be edited") {
                auto errors = db.get_errors(filters);
                REQUIRE(errors.size() == 1);
                REQUIRE(errors.front()->state == Everest::error::State::ClearedByModule);
            }
        }
        WHEN("Edit error severity") {
            std::list<Everest::error::ErrorFilter> filters = {
                Everest::error::ErrorFilter(Everest::error::HandleFilter(test_errors[4]->uuid))};
            Everest::error::ErrorDatabase::EditErrorFunc edit_func = [](Everest::error::ErrorPtr error) {
                error->severity = Everest::error::Severity::High;
            };
            REQUIRE(db.get_errors(filters).size() > 0);
            db.edit_errors(filters, edit_func);
            THEN("The error should be edited") {
                auto errors = db.get_errors(filters);
                REQUIRE(errors.size() == 1);
                REQUIRE(errors.front()->severity == Everest::error::Severity::High);
            }
        }
        WHEN("Edit error message") {
            std::list<Everest::error::ErrorFilter> filters = {
                Everest::error::ErrorFilter(Everest::error::HandleFilter(test_errors[4]->uuid))};
            Everest::error::ErrorDatabase::EditErrorFunc edit_func = [](Everest::error::ErrorPtr error) {
                error->message = "new_message";
            };
            REQUIRE(db.get_errors(filters).size() > 0);
            db.edit_errors(filters, edit_func);
            THEN("The error should be edited") {
                auto errors = db.get_errors(filters);
                REQUIRE(errors.size() == 1);
                REQUIRE(errors.front()->message == "new_message");
            }
        }
        WHEN("Edit error description") {
            std::list<Everest::error::ErrorFilter> filters = {
                Everest::error::ErrorFilter(Everest::error::HandleFilter(test_errors[4]->uuid))};
            Everest::error::ErrorDatabase::EditErrorFunc edit_func = [](Everest::error::ErrorPtr error) {
                error->description = "new_description";
            };
            REQUIRE(db.get_errors(filters).size() > 0);
            db.edit_errors(filters, edit_func);
            THEN("The error should be edited") {
                auto errors = db.get_errors(filters);
                REQUIRE(errors.size() == 1);
                REQUIRE(errors.front()->description == "new_description");
            }
        }
        WHEN("Edit error origin") {
            std::list<Everest::error::ErrorFilter> filters = {
                Everest::error::ErrorFilter(Everest::error::HandleFilter(test_errors[4]->uuid))};
            Everest::error::ErrorDatabase::EditErrorFunc edit_func = [](Everest::error::ErrorPtr error) {
                error->from = ImplementationIdentifier("new_from_module", "new_from_implementation");
            };
            REQUIRE(db.get_errors(filters).size() > 0);
            db.edit_errors(filters, edit_func);
            THEN("The error should be edited") {
                auto errors = db.get_errors(filters);
                REQUIRE(errors.size() == 1);
                REQUIRE(errors.front()->from == ImplementationIdentifier("new_from_module", "new_from_implementation"));
            }
        }
        WHEN("Edit error timestamp") {
            std::list<Everest::error::ErrorFilter> filters = {
                Everest::error::ErrorFilter(Everest::error::HandleFilter(test_errors[4]->uuid))};
            auto new_timestamp = date::utc_clock::now() + std::chrono::hours(10);
            Everest::error::ErrorDatabase::EditErrorFunc edit_func = [&new_timestamp](Everest::error::ErrorPtr error) {
                error->timestamp = new_timestamp;
            };
            REQUIRE(db.get_errors(filters).size() > 0);
            db.edit_errors(filters, edit_func);
            THEN("The error should be edited") {
                auto errors = db.get_errors(filters);
                REQUIRE(errors.size() == 1);
                REQUIRE(Everest::Date::to_rfc3339(errors.front()->timestamp) ==
                        Everest::Date::to_rfc3339(new_timestamp));
            }
        }
        WHEN("Edit error uuid") {
            Everest::error::UUID new_uuid;
            std::list<Everest::error::ErrorFilter> filters = {
                Everest::error::ErrorFilter(Everest::error::HandleFilter(test_errors[4]->uuid))};
            Everest::error::ErrorDatabase::EditErrorFunc edit_func = [&new_uuid](Everest::error::ErrorPtr error) {
                error->uuid = new_uuid;
            };
            REQUIRE(db.get_errors(filters).size() > 0);
            db.edit_errors(filters, edit_func);
            THEN("The error should be edited") {
                auto errors = db.get_errors(filters);
                REQUIRE(errors.size() == 0);
                errors = db.get_errors({Everest::error::ErrorFilter(Everest::error::HandleFilter(new_uuid))});
                REQUIRE(errors.size() == 1);
            }
        }

        WHEN("Remove error") {
            std::list<Everest::error::ErrorFilter> filters = {
                Everest::error::ErrorFilter(Everest::error::HandleFilter(test_errors[4]->uuid))};
            REQUIRE(db.get_errors(filters).size() > 0);
            db.remove_errors(filters);
            THEN("The error should be removed") {
                auto errors = db.get_errors(filters);
                REQUIRE(errors.size() == 0);
            }
        }
        WHEN("Remove multiple errors") {
            std::list<Everest::error::ErrorFilter> filters = {
                Everest::error::ErrorFilter(Everest::error::StateFilter::Active),
                Everest::error::ErrorFilter(
                    Everest::error::OriginFilter("test_from_module_c", "test_from_implementation_c"))};
            REQUIRE(db.get_errors(filters).size() > 0);
            db.remove_errors(filters);
            THEN("The errors should be removed") {
                auto errors = db.get_errors(filters);
                REQUIRE(errors.size() == 0);
            }
        }
        WHEN("Remove all errors") {
            std::list<Everest::error::ErrorFilter> filters = {};
            REQUIRE(db.get_errors(filters).size() > 0);
            db.remove_errors(filters);
            THEN("The errors should be removed") {
                auto errors = db.get_errors(filters);
                REQUIRE(errors.size() == 0);
            }
        }
    }
}
