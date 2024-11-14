#include <iostream>
#include <string>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <limits>
#include <vector>

using namespace std;

class WellnessBot {
private:
    // Constants for calculations
    struct ActivityMultiplier {
        string level;
        double multiplier;
    };

    const vector<ActivityMultiplier> ACTIVITY_MULTIPLIERS = {
        {"sedentary", 1.2},
        {"lightly active", 1.375},
        {"moderately active", 1.55},
        {"very active", 1.725}
    };

    struct MacroRatio {
        double carbs = 0.5;    // 50% of calories from carbs
        double protein = 0.2;  // 20% of calories from protein
        double fats = 0.3;     // 30% of calories from fats
    };

    const double CALORIES_PER_GRAM_PROTEIN = 4.0;
    const double CALORIES_PER_GRAM_CARBS = 4.0;
    const double CALORIES_PER_GRAM_FAT = 9.0;

    // BMI category thresholds
    struct BMIThresholds {
        const double underweight = 18.5;
        const double normal = 24.9;
        const double overweight = 29.9;
    } bmiThresholds;

    // Input validation functions
    static bool isValidGender(const string& gender) {
        string lower = gender;
        transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        return (lower == "male" || lower == "female");
    }

    static bool isValidActivityLevel(const string& level) {
        string lower = level;
        transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        return (lower == "sedentary" || lower == "lightly active" || 
                lower == "moderately active" || lower == "very active");
    }

    static bool isValidLifestyle(const string& lifestyle) {
        string lower = lifestyle;
        transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        return (lower == "smoking" || lower == "alcohol" || lower == "none");
    }

    static bool isValidDietaryPref(const string& pref) {
        string lower = pref;
        transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        return (lower == "vegetarian" || lower == "vegan" || lower == "none");
    }

    template<typename T>
    static T getValidInput(const string& prompt, T min_value, T max_value) {
        T value;
        while (true) {
            cout << prompt;
            if (cin >> value && value >= min_value && value <= max_value) {
                break;
            }
            cout << "Invalid input. Please enter a value between " 
                      << min_value << " and " << max_value << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        return value;
    }

    static string getValidStringInput(const string& prompt, 
                                         bool (*validationFunc)(const string&)) {
        string input;
        while (true) {
            cout << prompt;
            getline(cin, input);
            if (validationFunc(input)) {
                transform(input.begin(), input.end(), input.begin(), ::tolower);
                return input;
            }
            cout << "Invalid input. Please try again.\n";
        }
    }

public:
    struct UserProfile {
        int age;
        string gender;
        double height;  // in meters
        double weight;  // in kg
        string activityLevel;
        int sleepHours;
        string lifestyle;
        string dietaryPref;
        
        // Calculated values
        double bmi;
        double bmr;
        double dailyCalories;
    };

    UserProfile collectUserData() {
        UserProfile profile;
        
        profile.age = getValidInput<int>("Enter your age: ", 1, 120);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        profile.gender = getValidStringInput("Enter your gender (male/female): ", isValidGender);
        
        profile.height = getValidInput<double>("Enter your height (in meters): ", 0.5, 2.5);
        profile.weight = getValidInput<double>("Enter your weight (in kg): ", 20.0, 300.0);
        
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        profile.activityLevel = getValidStringInput(
            "Enter your activity level (sedentary, lightly active, moderately active, very active): ",
            isValidActivityLevel
        );
        
        profile.sleepHours = getValidInput<int>("Enter your hours of sleep per night: ", 0, 24);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        profile.lifestyle = getValidStringInput(
            "Enter your lifestyle habits (smoking, alcohol, none): ",
            isValidLifestyle
        );
        
        profile.dietaryPref = getValidStringInput(
            "Enter your dietary preferences (vegetarian, vegan, none): ",
            isValidDietaryPref
        );

        return profile;
    }

    void calculateMetrics(UserProfile& profile) {
        // Calculate BMI
        profile.bmi = profile.weight / pow(profile.height, 2);
        
        // Calculate BMR using Mifflin-St Jeor Equation
        if (profile.gender == "male") {
            profile.bmr = 88.362 + (13.397 * profile.weight) + 
                         (4.799 * profile.height * 100) - (5.677 * profile.age);
        } else {
            profile.bmr = 447.593 + (9.247 * profile.weight) + 
                         (3.098 * profile.height * 100) - (4.330 * profile.age);
        }

        // Calculate daily caloric needs
        for (const auto& activity : ACTIVITY_MULTIPLIERS) {
            if (activity.level == profile.activityLevel) {
                profile.dailyCalories = profile.bmr * activity.multiplier;
                break;
            }
        }
    }

    void displayResults(const UserProfile& profile) {
        cout << "\n=== Wellness Assessment Results ===\n\n";
        
        // Display BMI
        cout << "BMI: " << fixed << setprecision(2) << profile.bmi << " - ";
        if (profile.bmi < bmiThresholds.underweight)
            cout << "Category: Underweight\n";
        else if (profile.bmi < bmiThresholds.normal)
            cout << "Category: Normal weight\n";
        else if (profile.bmi < bmiThresholds.overweight)
            cout << "Category: Overweight\n";
        else
            cout << "Category: Obese\n";

        // Display BMR and daily caloric needs
        cout << "\nBMR: " << fixed << setprecision(2) 
                  << profile.bmr << " calories/day\n";
        cout << "Daily Caloric Needs: " << profile.dailyCalories << " calories\n";

        // Calculate and display macronutrients
        MacroRatio macros;
        cout << "\nRecommended Macronutrient Distribution:";
        cout << "\n  - Carbohydrates: " << (profile.dailyCalories * macros.carbs / 
                     CALORIES_PER_GRAM_CARBS) << " grams";
        cout << "\n  - Protein: " << (profile.dailyCalories * macros.protein / 
                     CALORIES_PER_GRAM_PROTEIN) << " grams";
        cout << "\n  - Fats: " << (profile.dailyCalories * macros.fats / 
                     CALORIES_PER_GRAM_FAT) << " grams\n";

        provideRecommendations(profile);
    }

    void provideRecommendations(const UserProfile& profile) {
        cout << "\n=== Personalized Recommendations ===\n";

        // Exercise recommendations
        cout << "\nExercise Recommendations:\n";
        if (profile.bmi >= bmiThresholds.normal) {
            cout << "- Start with low-impact activities like walking or swimming\n";
            cout << "- Aim for 150 minutes of moderate activity per week\n";
            cout << "- Include strength training 2-3 times per week\n";
        } else {
            cout << "- Maintain a balanced exercise routine\n";
            cout << "- Mix cardio with strength training\n";
            cout << "- Consider adding flexibility exercises\n";
        }

        // Sleep recommendations
        cout << "\nSleep Recommendations:\n";
        if (profile.sleepHours < 7) {
            cout << "- Aim to increase sleep to 7-8 hours per night\n";
            cout << "- Establish a regular sleep schedule\n";
            cout << "- Create a relaxing bedtime routine\n";
        } else {
            cout << "- Maintain your good sleep habits\n";
            cout << "- Consider sleep quality improvements\n";
        }

        // Dietary recommendations
        cout << "\nNutritional Recommendations:\n";
        if (profile.dietaryPref == "vegetarian") {
            cout << "- Focus on complete protein sources (eggs, dairy, legumes)\n";
            cout << "- Monitor B12 and iron intake\n";
        } else if (profile.dietaryPref == "vegan") {
            cout << "- Ensure adequate B12 supplementation\n";
            cout << "- Combine protein sources for complete amino acids\n";
            cout << "- Monitor iron, calcium, and vitamin D intake\n";
        } else {
            cout << "- Choose lean protein sources\n";
            cout << "- Include a variety of colorful vegetables\n";
            cout << "- Limit processed foods\n";
        }

        // Lifestyle recommendations
        if (profile.lifestyle != "none") {
            cout << "\nLifestyle Recommendations:\n";
            if (profile.lifestyle == "smoking") {
                cout << "- Consider smoking cessation programs\n";
                cout << "- Consult healthcare provider about cessation aids\n";
            } else if (profile.lifestyle == "alcohol") {
                cout << "- Limit alcohol consumption\n";
                cout << "- Consider alcohol-free days\n";
                cout << "- Stay hydrated\n";
            }
        }
    }
};

int main() {
    cout << "Welcome to the Wellness Bot!\n"
              << "============================\n\n";
    
    WellnessBot bot;
    try {
        auto profile = bot.collectUserData();
        bot.calculateMetrics(profile);
        bot.displayResults(profile);
        
        cout << "\nThank you for using Wellness Bot! Stay healthy!\n";
    }
    catch (const exception& e) {
        cerr << "An error occurred: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}