# Study the API inside inflection

There are 24 main public classes defined in the inflection library

~/inflection/inflection$ find build/inflection_headers |egrep hpp|xargs egrep -s INFLECTION_CLASS_API|egrep -v "util|exception"|egrep :class
```
build/inflection_headers/inflection/Object.hpp:class INFLECTION_CLASS_API inflection::Object {
build/inflection_headers/inflection/resources/DataRegistrationService.hpp:class INFLECTION_CLASS_API inflection::resources::DataRegistrationService final
build/inflection_headers/inflection/dictionary/DictionaryKeyIterator.hpp:class INFLECTION_CLASS_API inflection::dictionary::DictionaryKeyIterator
build/inflection_headers/inflection/dictionary/DictionaryMetaData.hpp:class INFLECTION_CLASS_API inflection::dictionary::DictionaryMetaData
build/inflection_headers/inflection/lang/features/LanguageGrammarFeatures_GrammarFeatures.hpp:class INFLECTION_CLASS_API inflection::lang::features::LanguageGrammarFeatures_GrammarFeatures
build/inflection_headers/inflection/lang/features/LanguageGrammarFeatures_Feature.hpp:class INFLECTION_CLASS_API inflection::lang::features::LanguageGrammarFeatures_Feature
build/inflection_headers/inflection/lang/features/LanguageGrammarFeatures_GrammarCategory.hpp:class INFLECTION_CLASS_API inflection::lang::features::LanguageGrammarFeatures_GrammarCategory
build/inflection_headers/inflection/lang/features/LanguageGrammarFeatures.hpp:class INFLECTION_CLASS_API inflection::lang::features::LanguageGrammarFeatures
build/inflection_headers/inflection/dialog/CommonConceptFactoryProvider.hpp:class INFLECTION_CLASS_API inflection::dialog::CommonConceptFactoryProvider
build/inflection_headers/inflection/dialog/LocalizedCommonConceptFactoryProvider.hpp:class INFLECTION_CLASS_API inflection::dialog::LocalizedCommonConceptFactoryProvider
build/inflection_headers/inflection/dialog/SemanticFeature.hpp:class INFLECTION_CLASS_API inflection::dialog::SemanticFeature
build/inflection_headers/inflection/dialog/SemanticConcept.hpp:class INFLECTION_CLASS_API inflection::dialog::SemanticConcept
build/inflection_headers/inflection/dialog/SpeakableConcept.hpp:class INFLECTION_CLASS_API inflection::dialog::SpeakableConcept
build/inflection_headers/inflection/dialog/SemanticFeatureModel_DisplayData.hpp:class INFLECTION_CLASS_API inflection::dialog::SemanticFeatureModel_DisplayData
build/inflection_headers/inflection/dialog/DisplayValue.hpp:class INFLECTION_CLASS_API inflection::dialog::DisplayValue
build/inflection_headers/inflection/dialog/SemanticConceptList.hpp:class INFLECTION_CLASS_API inflection::dialog::SemanticConceptList
build/inflection_headers/inflection/dialog/PronounConcept.hpp:class INFLECTION_CLASS_API inflection::dialog::PronounConcept
build/inflection_headers/inflection/dialog/InflectableStringConcept.hpp:class INFLECTION_CLASS_API inflection::dialog::InflectableStringConcept
build/inflection_headers/inflection/dialog/SemanticValue.hpp:class INFLECTION_CLASS_API inflection::dialog::SemanticValue
build/inflection_headers/inflection/dialog/SpeakableString.hpp:class INFLECTION_CLASS_API inflection::dialog::SpeakableString
build/inflection_headers/inflection/dialog/CommonConceptFactory.hpp:class INFLECTION_CLASS_API inflection::dialog::CommonConceptFactory
build/inflection_headers/inflection/dialog/NumberConcept.hpp:class INFLECTION_CLASS_API inflection::dialog::NumberConcept
build/inflection_headers/inflection/dialog/SemanticFeatureConceptBase.hpp:class INFLECTION_CLASS_API inflection::dialog::SemanticFeatureConceptBase
build/inflection_headers/inflection/dialog/SemanticFeatureModel.hpp:class INFLECTION_CLASS_API inflection::dialog::SemanticFeatureModel final
```
