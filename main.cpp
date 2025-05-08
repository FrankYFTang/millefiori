
#include <iostream>

#include "inflection/dialog/InflectableStringConcept.hpp"
#include "inflection/dialog/LocalizedCommonConceptFactoryProvider.hpp"
#include "inflection/dialog/SemanticFeatureModel.hpp"
#include "inflection/dialog/SpeakableString.hpp"
#include "inflection/message2/MF2Factory.hpp"
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

int main() {
    UErrorCode errorCode = U_ZERO_ERROR;

    std::map<UnicodeString, Formattable> arguments;
    arguments["name"]= Formattable("gato");

    auto customRegistry = MFFunctionRegistry::Builder(errorCode)
                    .adoptFormatter(FunctionName("inflection"),
                                    inflection::message2::MF2Factory::CreateFormatterFactory(), errorCode)
                    .adoptSelector(FunctionName("inflection"),
                                   inflection::message2::MF2Factory::CreateSelectorFactory(), errorCode)
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
 
    MessageFormatter mf2 = MessageFormatter::Builder(errorCode)
                .setFunctionRegistry(customRegistry)
                .setLocale(Locale::forLanguageTag("es-MX", errorCode))
                .setPattern(".local $var1 = {$name :inflection feature=gender} \
                             .local $var2 = {$name :inflection feature=number} \
                             .match $var1 $var2\
                             masculine 2 {{ Masculine 2 {$name} }} \
                             feminine singular {{ Feminine singular {$name} }} \
                             foo 4 {{ foo 4 {$name} }} \
                             masculine singular {{ Masculine singular {$name}}} \
                             hello singular {{ hello singular {$name}}} \
                             * * {{other {$name} }}\n",
                             pe, errorCode)
                .build(errorCode);
    ret = mf2.formatToString(MessageArguments(arguments, errorCode), errorCode);
    utf8.clear();
    std::cout << ret.toUTF8String<std::string>(utf8).c_str() << std::endl;
    //  output "Masculine singular gato"
}
