#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <limits>
#include <chrono>
#include <thread>

using namespace std;
using namespace std::chrono;

class Question {
public:
    string text;
    vector<string> options;
    int correctAnswerIndex;

    Question() {}

    Question(string t, vector<string> opts, int correctIndex)
        : text(t), options(opts), correctAnswerIndex(correctIndex) {}
};

class Quiz {
public:
    string title;
    vector<Question> questions;

    Quiz() {}

    Quiz(string t) : title(t) {}

    void addQuestion(Question q) {
        questions.push_back(q);
    }

    void takeQuiz() {
        int score = 0;
        for (size_t i = 0; i < questions.size(); ++i) {
            cout << "\nQ" << (i + 1) << ": " << questions[i].text << endl;
            for (size_t j = 0; j < questions[i].options.size(); ++j) {
                cout << j + 1 << ". " << questions[i].options[j] << endl;
            }

            int answer;
            cout << "Your answer (1-" << questions[i].options.size() << "): ";

            // Start timer for 30 seconds
            auto start_time = high_resolution_clock::now();
            auto time_limit = 30;  // Time limit in seconds

            while (true) {
                if (cin.peek() != '\n') {
                    cin >> answer;
                    break;
                }

                // Check if the time limit has exceeded
                auto elapsed_time = duration_cast<seconds>(high_resolution_clock::now() - start_time).count();
                if (elapsed_time >= time_limit) {
                    cout << "\nTime's up! Moving to the next question." << endl;
                    answer = -1;  // Default answer for time-out
                    break;
                }
                this_thread::sleep_for(milliseconds(1));  // Sleep for a short time to avoid busy-waiting
            }

            // Check if the answer is correct
            if (answer - 1 == questions[i].correctAnswerIndex) {
                score++;
            }
        }
        cout << "\nQuiz complete! Your score: " << score << "/" << questions.size() << endl;
    }
};

class Teacher {
public:
    vector<Quiz> quizzes;

    void createQuiz() {
        string title;
        cout << "\nEnter quiz title: ";
        cin.ignore();
        getline(cin, title);
        Quiz quiz(title);

        char addMore;
        do {
            string questionText;
            cout << "Enter question text: ";
            getline(cin, questionText);

            vector<string> options;
            for (int i = 0; i < 4; ++i) {
                string option;
                cout << "Enter option " << (i + 1) << ": ";
                getline(cin, option);
                options.push_back(option);
            }

            int correctIndex;
            cout << "Enter the index of the correct answer (1-4): ";
            cin >> correctIndex;

            while (cin.fail() || correctIndex < 1 || correctIndex > 4) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Enter a number between 1 and 4: ";
                cin >> correctIndex;
            }

            cin.ignore();
            quiz.addQuestion(Question(questionText, options, correctIndex - 1));

            cout << "Add another question? (y/n): ";
            cin >> addMore;
            cin.ignore();
        } while (addMore == 'y' || addMore == 'Y');

        quizzes.push_back(quiz);
        saveQuizzesToFile("data.txt");
        cout << "Quiz created and saved successfully!\n";
    }

    void viewQuizzes() {
        if (quizzes.empty()) {
            cout << "No quizzes available.\n";
            return;
        }

        cout << "\nAvailable Quizzes:\n";
        for (size_t i = 0; i < quizzes.size(); ++i) {
            cout << i + 1 << ". " << quizzes[i].title << endl;
        }
    }

    vector<Quiz>& getQuizzes() {
        return quizzes;
    }

    void saveQuizzesToFile(const string& filename) {
        ofstream outFile(filename);
        for (const Quiz& quiz : quizzes) {
            outFile << quiz.title << '\n';
            outFile << quiz.questions.size() << '\n';
            for (const Question& q : quiz.questions) {
                outFile << q.text << '\n';
                for (const string& opt : q.options) {
                    outFile << opt << '\n';
                }
                outFile << q.correctAnswerIndex << '\n';
            }
        }
        outFile.close();
    }

    void loadQuizzesFromFile(const string& filename) {
        ifstream inFile(filename);
        if (!inFile.is_open()) {
            cout << "No previous quizzes found.\n";
            return;
        }

        quizzes.clear();
        string title;
        while (getline(inFile, title)) {
            Quiz quiz(title);
            int numQuestions;
            inFile >> numQuestions;
            inFile.ignore();
            for (int i = 0; i < numQuestions; ++i) {
                string qText;
                getline(inFile, qText);
                vector<string> opts;
                for (int j = 0; j < 4; ++j) {
                    string opt;
                    getline(inFile, opt);
                    opts.push_back(opt);
                }
                int correctIndex;
                inFile >> correctIndex;
                inFile.ignore();
                quiz.addQuestion(Question(qText, opts, correctIndex));
            }
            quizzes.push_back(quiz);
        }

        inFile.close();
    }
};

class Student {
public:
    void takeQuiz(vector<Quiz>& quizzes) {
        if (quizzes.empty()) {
            cout << "\nNo quizzes available to take.\n";
            return;
        }

        cout << "\nAvailable Quizzes:\n";
        for (size_t i = 0; i < quizzes.size(); ++i) {
            cout << i + 1 << ". " << quizzes[i].title << endl;
        }

        int choice;
        cout << "Enter the number of the quiz you want to take: ";
        cin >> choice;

        while (cin.fail() || choice < 1 || choice > (int)quizzes.size()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid choice. Please enter a valid number: ";
            cin >> choice;
        }

        quizzes[choice - 1].takeQuiz();
    }
};

int main() {
    Teacher teacher;
    Student student;
    string userType;

    teacher.loadQuizzesFromFile("data.txt"); // 🔑 This is the key to enabling student mode

    cout << "Are you a student or a teacher? (student/teacher): ";
    cin >> userType;

    if (userType == "teacher") {
        int choice;
        do {
            cout << "\nTeacher Menu:\n";
            cout << "1. Create Quiz\n2. View Quizzes\n3. Exit\nChoose an option: ";
            cin >> choice;

            while (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please try again: ";
                cin >> choice;
            }

            switch (choice) {
                case 1:
                    teacher.createQuiz();
                    break;
                case 2:
                    teacher.viewQuizzes();
                    break;
                case 3:
                    cout << "Exiting teacher mode...\n";
                    break;
                default:
                    cout << "Invalid option. Please try again.\n";
            }
        } while (choice != 3);
    }
    else if (userType == "student") {
        student.takeQuiz(teacher.getQuizzes());
    }
    else {
        cout << "Invalid input. Please enter 'student' or 'teacher'.\n";
    }

    return 0;
}
