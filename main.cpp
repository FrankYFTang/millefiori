
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

class InflectionFormatterFactory : public FormatterFactory {
    public:
     Formatter* createFormatter(const Locale&, UErrorCode&) override;
};

class InflectionFormatter : public Formatter {
    public:
     FormattedPlaceholder format(FormattedPlaceholder&&, FunctionOptions&& opts, UErrorCode& errorCode) const override;
     // static MFFunctionRegistry customRegistry(UErrorCode&);
     InflectionFormatter(const inflection::dialog::SemanticFeatureModel* model) : model(model) { }
    private:
     const ::inflection::dialog::SemanticFeatureModel* model;
};

Formatter* InflectionFormatterFactory::createFormatter(const Locale& locale, UErrorCode& errorCode) {
    if (U_FAILURE(errorCode)) {
        return nullptr;
    }

    inflection::util::ULocale ulocale(locale.getLanguage(), locale.getCountry());
    auto model = ::inflection::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider()
                ->getCommonConceptFactory(ulocale)->getSemanticFeatureModel();
    Formatter* result = new InflectionFormatter(model);
    if (result == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
    }
    return result;
}

FormattedPlaceholder InflectionFormatter::format(FormattedPlaceholder&& arg, FunctionOptions&& options, UErrorCode& errorCode) const {
    if (U_FAILURE(errorCode)) {
        return {};
    }

    // Argument must be present
    if (!arg.canFormat()) {
        errorCode = U_MF_FORMATTING_ERROR;
        return FormattedPlaceholder("inflection");
    }

    // Assumes the argument is not-yet-formatted
    const Formattable& toFormat = arg.asFormattable();
    UnicodeString result;

    switch (toFormat.getType()) {
        case UFMT_STRING: {
            inflection::dialog::SpeakableString input(toFormat.getString(errorCode));
            inflection::dialog::InflectableStringConcept stringConcept(model, input);
            for (const auto& [key, value] : options.getOptions()) {
                auto constraint = model->getFeature(key);
                if (constraint != nullptr) {
                    auto semanticValue = value.getString(errorCode);
                    stringConcept.putConstraint(*constraint, semanticValue);
                }
            }
            result += stringConcept.toSpeakableString()->getPrint();
            // result += UnicodeString(stringConcept.toSpeakableString()->getPrint());
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

int main() {
    UErrorCode errorCode = U_ZERO_ERROR;

    std::map<UnicodeString, Formattable> arguments;
    arguments["owner"]=Formattable("gato");

    auto customRegistry = MFFunctionRegistry::Builder(errorCode)
                    .adoptFormatter(FunctionName("inflection"), new InflectionFormatterFactory(), errorCode)
                    .build();
    UParseError pe;
    MessageFormatter mf = MessageFormatter::Builder(errorCode)
                .setLocale(Locale::forLanguageTag("es-MX", errorCode))
                .setFunctionRegistry(customRegistry)
                .setPattern("Cartea {$owner :inflection hello=world definiteness=definite number=plural gender=feminine}", pe, errorCode)
                .build(errorCode);

    UnicodeString ret = mf.formatToString(MessageArguments(arguments, errorCode), errorCode);
    // CHECK(ret == u"Cartea las gatas");
    std::string utf8;
    std::cout << ret.toUTF8String<std::string>(utf8).c_str() << std::endl;
 
}
