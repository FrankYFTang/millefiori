#include <iostream>

#include "inflection/dialog/InflectableStringConcept.hpp"
#include "inflection/dialog/LocalizedCommonConceptFactoryProvider.hpp"
#include "inflection/dialog/SemanticFeatureModel.hpp"
#include "inflection/dialog/SpeakableString.hpp"
#include "inflection/lang/features/LanguageGrammarFeatures.hpp"
#include "inflection/util/ULocale.hpp"

#include "unicode/locid.h"
#include "unicode/messageformat2.h"
#include "unicode/messageformat2_function_registry.h"
#include "unicode/messageformat2_formattable.h"

using icu::Locale;
using icu::UnicodeString;
using icu::message2::Formattable;
using icu::message2::Formatter;
using icu::message2::FormatterFactory;
using icu::message2::FormattedPlaceholder;
using icu::message2::FormattedValue;
using icu::message2::FunctionOptions;
using icu::message2::FunctionOptionsMap;
using icu::message2::MessageArguments;
using icu::message2::MessageFormatter;
using icu::message2::MFFunctionRegistry;
using icu::message2::data_model::FunctionName;
class GrammarCasesFormatterFactory : public FormatterFactory {
    public:
     Formatter* createFormatter(const Locale&, UErrorCode&) override;
};

class GrammarCasesFormatter : public Formatter {
    public:
    FormattedPlaceholder format(FormattedPlaceholder&&, FunctionOptions&& opts, UErrorCode& errorCode) const override;
    // static MFFunctionRegistry customRegistry(UErrorCode&);
    private:
    void getDativeAndGenitive(const UnicodeString&, UnicodeString& result) const;
};

Formatter* GrammarCasesFormatterFactory::createFormatter(const Locale& locale, UErrorCode& errorCode) {
    if (U_FAILURE(errorCode)) {
        return nullptr;
    }

    // Locale not used
    (void) locale;

    Formatter* result = new GrammarCasesFormatter();
    if (result == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
    }
    return result;
}

FormattedPlaceholder GrammarCasesFormatter::format(FormattedPlaceholder&& arg, FunctionOptions&& options, UErrorCode& errorCode) const {
    if (U_FAILURE(errorCode)) {
        return {};
    }

    // Argument must be present
    if (!arg.canFormat()) {
        errorCode = U_MF_FORMATTING_ERROR;
        return FormattedPlaceholder("grammarBB");
    }

    // Assumes the argument is not-yet-formatted
    const Formattable& toFormat = arg.asFormattable();
    UnicodeString result;

    FunctionOptionsMap opt = options.getOptions();
    switch (toFormat.getType()) {
        case UFMT_STRING: {
            const UnicodeString& in = toFormat.getString(errorCode);
            bool hasCase = opt.count("case") > 0;
            bool caseIsString = opt["case"].getType() == UFMT_STRING;
            if (hasCase && caseIsString && (opt["case"].getString(errorCode) == "dative" || opt["case"].getString(errorCode) == "genitive")) {
                getDativeAndGenitive(in, result);
            } else {
                result += in;
            }
            // U_ASSERT(U_SUCCESS(errorCode));
            break;
        }
        default: {
            result += toFormat.getString(errorCode);
            break;
        }
    }

    return FormattedPlaceholder(arg, FormattedValue(std::move(result)));
}

void GrammarCasesFormatter::getDativeAndGenitive(const UnicodeString& value, UnicodeString& result) const {
    UnicodeString postfix;
    if (value.endsWith("ana")) {
        value.extract(0,  value.length() - 3, postfix);
        postfix += "nei";
    }
    else if (value.endsWith("ca")) {
        value.extract(0, value.length() - 2, postfix);
        postfix += "căi";
    }
    else if (value.endsWith("ga")) {
        value.extract(0, value.length() - 2, postfix);
        postfix += "găi";
    }
    else if (value.endsWith("a")) {
        value.extract(0, value.length() - 1, postfix);
        postfix += "ei";
    }
    else {
        postfix = "lui " + value;
    }
    result += postfix;
}


int main() {
    UErrorCode errorCode = U_ZERO_ERROR;

    MFFunctionRegistry customRegistry = MFFunctionRegistry::Builder(errorCode)
        .adoptFormatter(FunctionName("grammarBB"), new GrammarCasesFormatterFactory(), errorCode)
        .build();

    UParseError pe;
    MessageFormatter mf = MessageFormatter::Builder(errorCode)
                .setLocale(Locale::forLanguageTag("ro", errorCode))
                .setFunctionRegistry(customRegistry)
                .setPattern("Cartea {$owner :grammarBB case=genitive}", pe, errorCode)
                .build(errorCode);

    std::map<UnicodeString, Formattable> arguments;
    arguments["owner"]=Formattable("Maria");
    UnicodeString ret = mf.formatToString(MessageArguments(arguments, errorCode), errorCode);

    std::string utf8;

    std::cout << ret.toUTF8String<std::string>(utf8).c_str() << std::endl;


    std::cout << "===============================================" << std::endl;
    std::cout << "getKnownLanguages" << std::endl;
    std::cout << "===============================================" << std::endl;
for (const auto& locale : ::inflection::lang::features::LanguageGrammarFeatures::getKnownLanguages()) {
    std::cout << "Locale = " << locale.getName() << std::endl;

    auto grammarFeatures(::inflection::lang::features::LanguageGrammarFeatures::getLanguageGrammarFeatures(locale));
    std::cout << "===============================================" << std::endl;
    std::cout << "getCategories" << std::endl;
    std::cout << "===============================================" << std::endl;
    auto categories(grammarFeatures.getCategories());
    for (const auto& [categoryName, category] : categories) {
        utf8.clear();
        UnicodeString name(categoryName);
        std::cout << name.toUTF8String<std::string>(utf8).c_str() << std::endl;
        for (const auto& value : category.getValues()) {
            utf8.clear();
            UnicodeString value2(value);
            std::cout << "    " << value2.toUTF8String<std::string>(utf8).c_str() << std::endl;
        }

    }

    std::cout << "===============================================" << std::endl;
    std::cout << "getFeatures" << std::endl;
    std::cout << "===============================================" << std::endl;
    auto features(grammarFeatures.getFeatures());
    for (const auto& feature : features) {
        utf8.clear();
        UnicodeString name(feature.getName());
        std::cout << name.toUTF8String<std::string>(utf8).c_str() << std::endl;
        for (const auto& value : feature.getValues()) {
            utf8.clear();
            UnicodeString value2(value.getValue());
            std::cout << "    " << value2.toUTF8String<std::string>(utf8).c_str() << std::endl;
            for (const auto& [k, v] : value.getConstraints()) {
                UnicodeString kname(k);
                UnicodeString vname(v);
                kname.toUTF8String<std::string>(utf8);
                utf8 += " -> ";
                vname.toUTF8String<std::string>(utf8);
                std::cout << "        " << utf8.c_str() << std::endl;
                utf8.clear();
            }
        }
    }


}

    inflection::util::ULocale locale("es-MX");
    auto model = ::inflection::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider()->getCommonConceptFactory(locale)->getSemanticFeatureModel();


    inflection::dialog::SpeakableString input(u"toquera");
    inflection::dialog::InflectableStringConcept stringConcept(model, input);
    utf8.clear();
    std::cout << "Input:  "
              << UnicodeString(input.getPrint()).toUTF8String<std::string>(utf8).c_str()
              << std::endl;


        auto features = ::inflection::lang::features::LanguageGrammarFeatures::getLanguageGrammarFeatures(locale);
        ::std::vector<inflection::dialog::SemanticFeature> constraints;
        ::std::vector<inflection::dialog::SemanticFeature> semanticFeatures;
        bool hasDisplayFunction = model->getDefaultDisplayFunction() != nullptr;
        for (const auto& [featureName, grammarCategory] : features.getCategories()) {
            if (featureName != u"pos" && featureName != u"sound" && grammarCategory.isUniqueValues()) {
                if (hasDisplayFunction) {
                    auto constraint = *(model->getFeature(featureName));
    utf8.clear();
    std::cout << std::endl
              << "---------------------------------------------------------------------"
              << "Feature: "
              << UnicodeString(featureName).toUTF8String<std::string>(utf8).c_str()
              << std::endl;

                    for (const auto& semanticValue : constraint.getBoundedValues()) {

    utf8.clear();
    std::cout << "["
              << UnicodeString(semanticValue).toUTF8String<std::string>(utf8).c_str()
              << "]:\t";

                        stringConcept.putConstraint(constraint, semanticValue);

    utf8.clear();
    std::cout << UnicodeString(stringConcept.toSpeakableString()->getPrint()).toUTF8String<std::string>(utf8).c_str()
              << std::endl;

                        stringConcept.clearConstraint(constraint);
                    }

                }
                // Can we query the value for gender, number, case or something like that?
                auto semanticFeature = model->getFeature(featureName);
                if (model->getDefaultFeatureFunction(*(semanticFeature)) != nullptr) {
                    semanticFeatures.push_back(*(semanticFeature));
                }
            }
            // else we don't support inflecting these other types, or they aren't interesting.
        }
        for (const auto& feature : features.getFeatures()) {
            auto semanticFeature = model->getFeature(feature.getName());
            if (model->getDefaultFeatureFunction(*(semanticFeature)) != nullptr) {
                semanticFeatures.push_back(*(semanticFeature));
            }
            // else we don't support inflecting with this semantic feature.
        }

    // stringConcept.putConstraint(constraint, semanticValue);
    return 0;
}

