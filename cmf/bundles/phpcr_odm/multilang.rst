.. highlight:: php
   :linenothreshold: 2

.. index::
    single: wielojęzyczność; DoctrinePHPCRBundle

Obsługa wielojezyczności w Doctrine PHPCR-ODM
=============================================

Korzystanie z funkcjonalności wielojęzyczności PHPCR-ODM wymaga włączenia
w konfiguracji ustawień regionalnych.

Konfiguracja tłumaczeń
----------------------

Stosowanie tłumaczeń dokumentów wymaga skonfigurowanie dostępnych języków:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config.yml
        doctrine_phpcr:
            odm:
                # ...
                locales:
                    en: [de, fr]
                    de: [en, fr]
                    fr: [en, de]
                locale_fallback: hardcoded

    .. code-block:: xml
       :linenos:

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services">

            <config xmlns="http://doctrine-project.org/schema/symfony-dic/odm/phpcr">

                <odm locale-fallback="hardcoded">
                    <!-- ... -->
                    <locale name="en">
                        <fallback>de</fallback>
                        <fallback>fr</fallback>
                    </locale>

                    <locale name="de">
                        <fallback>en</fallback>
                        <fallback>fr</fallback>
                    </locale>

                    <locale name="fr">
                        <fallback>en</fallback>
                        <fallback>de</fallback>
                    </locale>
                </odm>
            </config>
        </container>

    .. code-block:: php
       :linenos:

        // app/config/config.php
        $container->loadFromExtension('doctrine_phpcr', array(
            'odm' => array(
                // ...
                'locales' => array(
                    'en' => array('de', 'fr'),
                    'de' => array('en', 'fr'),
                    'fr' => array('en', 'de'),
                ),
                'locale_fallback' => 'hardcoded',
            )
        );

Tablica ``locales`` jest listą alternatywnych oznaczeń regionalnych do przeszukiwania,
jeśli dokument nie został przetłumaczony na żądany język.

Pakiet ten dostarcza detektor żądań, który jest aktywowany, gdy jest skonfigurowane
jakieś ustawienie regionalne. Detektor ten aktualizuje PHPCR-ODM, tak aby używał
ustawienia regionalnego Symfony, określonego w tym żądaniu, jeśli symbol takiego
języka znajduje się na liście kluczy ustalonych w ``locales``.

Strategie awaryjnych wyborów
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Istnieje kilka strategi ustalenia kolejności awaryjnego wyboru dla wybranych języków,
na podstawie listy akceptowanych oznaczeń regionalnych określonych w żądaniu
(określonych w Symfony w nagłówku ``Accept-Language`` HTML). Wszystkie z tych strategii
nigdy nie będą dodawać jakiegokolwiek języka nie skonfigurowanego w ustawieniach
regionalnych, aby uniknąć żądań wstrzykujących niepożądane rzeczy do repozytorium:

* ``hardcoded``: domyślna strategia nie aktualizująca kolejności awaryjnych
  wyborów w żądaniu;
* ``replace``: przyjmuje zaakceptowane w żądaniu języki i aktualizuje
  kolejność wyboru awaryjnego, usuwając wszystkie języki nie znalezione
  w żądaniu;
* ``merge``: to samo co ``replace``, ale dodaje języki nie znalezione w żądaniu,
  ale istniejące w konfiguracji ``locales`` powracając aż do końca listy rezerwowych
  języków. Zapisuje to języki utraty jakiegokolwiek z nich.

Tłumaczenia dokumentów
----------------------

Ustawienie tłumaczenia dokumentu wymaga zdefiniowania atrybutu ``translator``
w odwzorowaniu dokumentu i wymaga odwzorowania pola ``locale``. Następnie można
użyć atrybut ``translated`` we wszystkich polach, które powinny różnić się w
zależności od języka.

.. configuration-block::

    .. code-block:: php
       :linenos:

        <?php

        use Doctrine\ODM\PHPCR\Mapping\Annotations as PHPCR;

        /**
         * @PHPCR\Document(translator="attribute")
         */
        class MyPersistentClass
        {
            /**
             * The language this document currently is in
             * @PHPCR\Locale
             */
            private $locale;

            /**
             * Untranslated property
             * @PHPCR\Date
             */
            private $publishDate;

            /**
             * Translated property
             * @PHPCR\String(translated=true)
             */
            private $topic;

            /**
             * Language specific image
             * @PHPCR\Binary(translated=true)
             */
            private $image;
        }

    .. code-block:: xml
       :linenos:

        <doctrine-mapping>
            <document class="MyPersistentClass"
                      translator="attribute">
                <locale fieldName="locale" />
                <field fieldName="publishDate" type="date" />
                <field fieldName="topic" type="string" translated="true" />
                <field fieldName="image" type="binary" translated="true" />
            </document>
        </doctrine-mapping>

    .. code-block:: yaml
       :linenos:

        MyPersistentClass:
          translator: attribute
          locale: locale
          fields:
            publishDate:
                type: date
            topic:
                type: string
                translated: true
            image:
                type: binary
                translated: true

Jeśli jawnie wejdzie się w interakcję z funkcjonalnościami wielojęzyczności
PHPCR-ODM, dokumentu będą ładowane w ustawieniach regionalnych żądania i zapisywane
w lokalizacyjnym, w którym zostały załadowane. Mogą to być różne ustawienia regionalne,
jeśli PHPCR-ODM nie odnalazł żądanego języka i ma ustawiony wybór rezerwowy do
alternatywnego języka.

.. tip::

    Więcej informacji o dokumentach wielojęzycznych można znaleźć w
    `dokumentacji wielojęzyczności PHPCR-ODM`_.

.. _`dokumentacji wielojęzyczności PHPCR-ODM`: http://docs.doctrine-project.org/projects/doctrine-phpcr-odm/en/latest/reference/multilang.html
