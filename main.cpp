
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
using icu::message2::Selector;
using icu::message2::SelectorFactory;
using icu::message2::data_model::FunctionName;

class InflectionFormatterFactory : public FormatterFactory {
    public:
     Formatter* createFormatter(const Locale&, UErrorCode&) override;
};
class InflectionSelectorFactory : public SelectorFactory {
    public:
     Selector* createSelector(const Locale&, UErrorCode&) const override;
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

    Formatter* result = new InflectionFormatter(
        ::inflection::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider()
                ->getCommonConceptFactory(inflection::util::ULocale(locale.getLanguage(), locale.getCountry()))
                ->getSemanticFeatureModel());
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

class InflectionSelector : public Selector {
    public:
     void selectKey (FormattedPlaceholder &&arg, FunctionOptions &&options, const UnicodeString *keys, int32_t keysLen, UnicodeString *prefs, int32_t &prefsLen, UErrorCode &status) const override;
     InflectionSelector(const inflection::dialog::SemanticFeatureModel* model) : model(model) { }
    private:
     const ::inflection::dialog::SemanticFeatureModel* model;
};

Selector* InflectionSelectorFactory::createSelector(const Locale& locale, UErrorCode& errorCode) const {
    if (U_FAILURE(errorCode)) {
        return nullptr;
    }

    Selector* result = new InflectionSelector(
        ::inflection::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider()
               ->getCommonConceptFactory(inflection::util::ULocale(locale.getLanguage(), locale.getCountry()))
                ->getSemanticFeatureModel());
    if (result == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
    }
    return result;
}

void InflectionSelector::selectKey(
    FormattedPlaceholder &&arg, FunctionOptions &&options, const UnicodeString *keys, int32_t keysLen,
    UnicodeString *prefs, int32_t &prefsLen, UErrorCode &errorCode) const {
    if (U_FAILURE(errorCode)) { return; }
    // Argument must be present
    if (!arg.canFormat()) {
        errorCode = U_MF_SELECTOR_ERROR;
        return;
    }

    // Assumes the argument is not-yet-formatted
    const Formattable& toFormat = arg.asFormattable();
    UnicodeString result;
    prefsLen = 0;
    if (toFormat.getType() == UFMT_STRING) {
       inflection::dialog::SpeakableString input(toFormat.getString(errorCode));
       inflection::dialog::InflectableStringConcept stringConcept(model, input);
       if (!options.getOptions().contains(u"feature")) {
          errorCode = U_MF_SELECTOR_ERROR;
          return;
       }
       auto value = UnicodeString(stringConcept.getFeatureValue(
           *(model->getFeature(
               options.getOptions()
                      .at(u"feature")
                      .getString(errorCode))))->getPrint());

       for (int i = 0; i < keysLen; i++) {
           if (value == keys[i]) {
            prefs[prefsLen++] = keys[i];
           }
       }
    }


    return;
}

int main() {
    UErrorCode errorCode = U_ZERO_ERROR;

    std::map<UnicodeString, Formattable> arguments;
    arguments["name"]= Formattable("gato");

    auto customRegistry = MFFunctionRegistry::Builder(errorCode)
                    .adoptFormatter(FunctionName("inflection"), new InflectionFormatterFactory(), errorCode)
                    .adoptSelector(FunctionName("inflection"), new InflectionSelectorFactory(), errorCode)
                    .build();
    UParseError pe;
    MessageFormatter mf = MessageFormatter::Builder(errorCode)
                .setFunctionRegistry(customRegistry)
                .setLocale(Locale::forLanguageTag("es-MX", errorCode))
                // Copy from
                // https://github.com/unicode-org/inflection/blob/main/inflection/test/resources/inflection/dialog/inflection/es.xml#L120C77-L120C81
                .setPattern("Cartea {$name :inflection hello=world definiteness=definite number=plural gender=feminine}", pe, errorCode)
                .build(errorCode);

    UnicodeString ret = mf.formatToString(MessageArguments(arguments, errorCode), errorCode);
    // CHECK(ret == u"Cartea las gatas");
    std::string utf8;
    std::cout << ret.toUTF8String<std::string>(utf8).c_str() << std::endl;
 
    std::cout << "hi" << std::endl;
    std::cout << errorCode << std::endl;
    MessageFormatter mf2 = MessageFormatter::Builder(errorCode)
                .setFunctionRegistry(customRegistry)
                .setLocale(Locale::forLanguageTag("es-MX", errorCode))
                .setPattern(".local $var1 = {$name :inflection feature=gender} \
                             .local $var2 = {$name :inflection feature=number} \
                             .match $var1 $var2\
                             masculine singular {{ Masculine 0 {$name}}} \
                             masculine 2 {{ Masculine 1}} \
                             feminine singular {{ Feminine 2}} \
                             foo  4 {{ foo bar}} \
                             hello singular {{ hello $name}} \
                             * * {{other }}\n",
                             pe, errorCode)
                .build(errorCode);
    std::cout << errorCode << std::endl;
    ret = mf2.formatToString(MessageArguments(arguments, errorCode), errorCode);
    std::cout << errorCode << std::endl;
    utf8.clear();
    std::cout << ret.toUTF8String<std::string>(utf8).c_str() << std::endl;
    std::cout << errorCode << std::endl;
}
