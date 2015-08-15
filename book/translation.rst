.. highlight:: php
   :linenothreshold: 2

.. index::
   tłumaczenia, umiedzynarodowienie
   single: tłumaczenia; ustawienia narodowe
   single: tłumaczenia; identyfikator narodowy

Tłumaczenia
===========

Pojęcie **umiędzynarodowienie** (*ang. internationalization*) (często skracane do `i18n`_)
odnosi się do procesu uabstrakcyjnienia łańcuchów tekstowych i innych elementów
aplikacji specyficznych dla języka, tworzących warstwę aplikacji, w której te abstrakcyjne
wyrażenia mogą być tłumaczone i przekształcane na wyrażenia specyficzne dla
**ustawienia narodowego** użytkownika. W przypadku tekstu
oznacza to opakowanie tekstu w funkcję zdolną do translacji tekstu (lub "komunikatu")
na język użytkownika::

    // ten tekst zawsze będzie drukowany w języku angielskim
    echo 'Hello World';

    // ten tekst może zostać przetłumaczony na język użytkownika końcowego lub
    // domyślnie na język angielski
    echo $translator->trans('Hello World');

.. note::

    Pojęcie **ustawienia narodowego** (*ang. locale*), czesto też nazywanego ustawieniem
    regionalnym, dotyczy mniej więcej języka i kraju użytkownika. Oznaczane jest
    przez jakiś łańcuch tekstowy, który aplikacja
    używa do zarządzania tłumaczeniami i innymi różnicami formatów (np. formatem walutowym).
    Zalecanym oznaczeniem ustawień narodowych jest format złożony z kodu językowego
    wg. `ISO639-1`_, znaku podkreślenia (``_``) i następnie kodu kraju
    wg. `ISO3166 alpha-2`_ (np. ``pl_PL`` dla polski/Polska).
    Oznaczenie ustawienia narodowego w aplikacji będziemy w tym podręczniku nazywać
    **identyfikatorem narodowym**.
    
W tym rozdziale dowiesz się jak wykorzystywać w Symfony2  komponent Translation.
Przeczytaj :doc:`dokumentację komponentu Translation </components/translation/usage>`
dla dowiedzenia się więcej na ten temat. Ogólnie rzecz biorąc, proces implementacji
funkcjonalności tłumaczeń składa się z kilku kroków:

#. :ref:`Włączenie i skonfigurowanie <book-translation-configuration>` usługi
   tłumaczeń Symfony;

#. Uabstrakcyjnienie ciągów tekstowych (tj. "komunikatów") przez opakowanie ich
   w wywołaniach ``translator`` (":ref:`book-translation-basic`");

#. :ref:`Utworzenie zasobów (plików) translacyjnych <book-translation-resources>`
   dla każdego obsługiwanego ustawienia narodowego, w którym przetłumaczony jest
   każdy komunikat stosowany w aplikacji;

#. Ustalenie, :ref:`skonfigurowanie i zarządzanie ustawieniem narodowym użytkownika
   <book-translation-user-locale>` dla żądania i ewentualnie :doc:`dla całej sesji
   użytkownika </cookbook/session/locale_sticky_session>`. 

.. index::
   single: tłumaczenia; konfiguracja

.. _book-translation-configuration:

Konfiguracja
------------

Tłumaczenia są obsługiwane przez :term:`usługę <usługa>` ``translator``, która
wykorzystuje ustawienia narodowe użytkownika do wyszukania i zwrócenia
przetłumaczonych komunikatów. Przed zastosowanie tego trzeba udostępnić ``translator``
w konfiguracji:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config.yml
        framework:
            translator: { fallback: en }

    .. code-block:: xml
       :linenos:

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd
                                http://symfony.com/schema/dic/symfony http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:translator fallback="en" />
            </framework:config>
        </container>

    .. code-block:: php
       :linenos:

        // app/config/config.php
        $container->loadFromExtension('framework', array(
            'translator' => array('fallback' => 'en'),
        ));

W rozdziale :ref:`book-translation-fallback` omówiono szczegóły dotyczace klucza
``fallback`` i to, co się dzieje, gdy Symfony nie może znaleźć tłumaczenia.

Identyfikator narodowy wykorzystywany w tłumaczeniach jest przechowywane w żądaniu.
Zazwyczaj jest ustawiane w trasie przez atrybut ``_locale``
(zobacz :ref:`book-translation-locale-url`).

.. _book-translation-basic:

Podstawowe tłumaczenie
----------------------

Tłumaczenie tekstu jest realizowane przez usługę ``translator``
(:class:`Symfony\\Component\\Translation\\Translator`). Aby przetłumaczyć blok
tekstu (nazywany tu *komunikatem*), trzeba użyć metody
:method:`Symfony\\Component\\Translation\\Translator::trans`.
Załóżmy na przykład, że tłumaczymy prosty komunikat wewnątrz kontrolera::

    // ...
    use Symfony\Component\HttpFoundation\Response;

    public function indexAction()
    {
        $translated = $this->get('translator')->trans('Symfony2 is great');

        return new Response($translated);
    }

.. _book-translation-resources:

Podczas wykonywani tego kodu, Symfony2 będzie próbował przetłumaczyć komunikat
"Symfony2 is great" w oparciu o ustawienie narodowe użytkownika. Będzie to działało,
jeśli powiadomi się Symfony2 jak ma przetłumaczyć komunikat udostępniając "zasób
translacyjny", którym zwykle jest plik zawierający kolekcje tłumaczeń dla określonego
ustawienia narodowego. Ten "słownik" tłumaczeń może zostać stworzony w różnych
formatach. Zalecanym formatem jest XLIFF:

.. configuration-block::

    .. code-block:: xml
       :linenos:

        <!-- messages.pl.xliff -->
        <?xml version="1.0"?>
        <xliff version="1.2" xmlns="urn:oasis:names:tc:xliff:document:1.2">
            <file source-language="en" datatype="plaintext" original="file.ext">
                <body>
                    <trans-unit id="1">
                        <source>Symfony2 is great</source>
                        <target>Symfony2 jest wielkie</target>
                    </trans-unit>
                </body>
            </file>
        </xliff>

    .. code-block:: php
       :linenos:

        // messages.pl.php
        return array(
            'Symfony2 is great' => 'Symfony2 jest wielkie',
        );

    .. code-block:: yaml
       :linenos:

        # messages.pl.yml
        Symfony2 is great: Symfony2 jest wielkie

Informacja o tym, gdzie powinny być umieszczone te pliki translacyjne, znajduje
się w rozdziale :ref:`book-translation-resource-locations`.

Teraz, gdy językiem ustawienia narodowego użytkownika jest język polski, to komunikat
zostanie przetłumaczony jako ``Symfony2 jest wielkie``.
Można również przetłumaczyć komunikat wewnątrz :ref:`szablonów <book-translation-tags>`.

.. index::
   single: tłumaczenia; zasób translacyjny

Proces tłumaczenia
~~~~~~~~~~~~~~~~~~

W celu właściwego przetłumaczenia komunikatu, Symfony2 wykonuje następujące czynności:

* Zostaje określony identyfikator narodowy bieżącego użytkownika, który
  jest zawarty w żądaniu (lub przechowywany jako wartość ``_locale`` w sesji);

* Ze zasobów translacyjnych dla określonej wartości identyfikatora narodowego
  (np. ``pl_PL``) ładowany jest katalog przetłumaczonych komunikatów. Ładowane
  są również komunikaty dla :ref:`podstawowego identyfikatora narodowego
  <book-translation-fallback>` i dodawane są one do katalogu jeśli jeszcze w nim nie
  istnieją. Końcowym rezultatem jest wielki "słownik" tłumaczeń w postaci katalogu
  komunikatów (*ang. message catalogue*). Szczegóły omówione są w rozdziale
  :ref:`message-catalogues`.

* Jeśli komunikat znajduje się w katalogu, to zwracane jest tłumaczenie. Jeśli nie,
  to zwracany jest oryginalny komunikat.

Gdy używa się metody ``trans()``, Symfony2 wyszukuje dokładny łańcuch tekstowy w
odpowiednim katalogu komunikatów i go zwraca (jeśli istnieje).

.. index::
   single: tłumaczenia; komunikaty zastępcze
   single: tłumaczenia; zasób translacyjny

Symbole zastępcze w komunikatach
--------------------------------

Czasem komunikat zwiera zmienną, która musi być tłumaczona::

    use Symfony\Component\HttpFoundation\Response;

    public function indexAction($name)
    {
        $translated = $this->get('translator')->trans('Hello '.$name);

        return new Response($translated);
    }


Jednak utworzenie tłumaczenia tego łańcucha nie jest możliwe, gdyż translator
będzie próbował wyszukać komunikat łącznie z wartościa tekstową zmiennej
(np. "Hello Ryan" lub "Hello Fabien").

Szczegóły o tym, jak obsługiwać taką sytuację znajduje się
w :ref:`component-translation-placeholders` w dokumentacji komponentu.
Informacja o tym, jak to zrobić w szablonach znajduje się w :ref:`book-translation-tags`.

.. index::
   single: tłumaczenia; liczba mnoga

Liczba mnoga
------------

Inną komplikacją tłumaczeń jest problem liczby mnogiej – różnej wartości tej samej
zmiennej, w zależności od formy liczby mnogiej, która jest też różna w zależności
od języka:

.. code-block:: text

    There is one apple.
    There are 5 apples.
    
    To jest jabłko.
    To są dwa jabłka.
    To jest pięć jabłek.

W celu obsługi liczby mnogiej trzeba użyć metodę
:method:`Symfony\\Component\\Translation\\Translator::transChoice`
lub znacznika (filtr) ``transchoice`` w :ref:`szablonie <book-translation-tags>`.

Więcej informacji znajduje się w
:ref:`dokumentacji komponentu Translation <component-translation-pluralization>`.

.. index::
   single: tłumaczenia; w szablonach

Tłumaczenia w szablonach
------------------------

W większości przypadków, tłumaczenia dokonywane są w szablonach. Symfony2 dostarcza
natywną obsługę tłumaczeń zarówno w szablonach Twig jak i PHP.

.. index::
   single: szablony Twig; tłumaczenia

.. _book-translation-tags:

Szablony Twig
~~~~~~~~~~~~~

Symfony2 zapewnia wyspecjalizowane znaczniki Twig (``trans`` i ``transchoice``)
w celu pomocy z tłumaczeniem komunikatów *statycznych bloków tekstu*:

.. code-block:: jinja
   :linenos:

    {% trans %}Hello %name%{% endtrans %}

    {% transchoice count %}
        {0} There are no apples|{1} There is one apple|]1,Inf] There are %count% apples
    {% endtranschoice %}

Znacznik ``transchoice`` automatycznie pobiera zmienną ``%count%`` z bieżącego
kontekstu i przekazuje ją do translatora. Mechanizm ten działa tylko w przypadku
użycia symbolu zastępczego zgodnego ze wzorcem ``%var%``.

.. caution::

    Podczas tłumaczeń w szablonach Twig przy użyciu znaczników wymagana jest
    notacja symboli zamiennych ``%var%``.

.. tip::

    Jeśli w ciągu musi się użyć znak procenta  (``%``), to trzeba go zabezpieczyć
    stosując podwójne znaki procent: ``{% trans %}Procent: %percent%%%{% endtrans %}``

Można również określić domenę komunikatu i przekazać kilka dodatkowych zmiennych:

.. code-block:: jinja
   :linenos:

    {% trans with {'%name%': 'Fabien'} from "app" %}Hello %name%{% endtrans %}

    {% trans with {'%name%': 'Fabien'} from "app" into "fr" %}Hello %name%{% endtrans %}

    {% transchoice count with {'%name%': 'Fabien'} from "app" %}
        {0} %name%, there are no apples|{1} %name%, there is one apple|]1,Inf] %name%, there are %count% apples
    {% endtranschoice %}

.. _book-translation-filters:

Filtru ``trans`` i ``transchoice`` mogą zostać użyte do przetłumaczenia *tekstów
zmiennych* i złożonych wyrażeń:

.. code-block:: jinja
   :linenos:

    {{ message|trans }}

    {{ message|transchoice(5) }}

    {{ message|trans({'%name%': 'Fabien'}, "app") }}

    {{ message|transchoice(5, {'%name%': 'Fabien'}, 'app') }}

.. tip::

    Stosowanie znaczników translacyjnych i filtrów daje ten sam efekt, ale z jedną
    subtelna różnicą: automatyczne zabezpieczenie danych wyjściowych jest stosowane
    tylko w tłumaczeniach wykorzystujących filtry. Innymi słowami, jeśli chce się mieć
    pewność, że przetłumaczony komunikat nie został zabezpieczony na wyjściu znakami
    ucieczki, trzeba zastosować filtr `raw`` po filtrze translacyjnym:

    .. code-block:: jinja
       :linenos:

            {# text translated between tags is never escaped #}
            {% trans %}
                <h3>foo</h3>
            {% endtrans %}

            {% set message = '<h3>foo</h3>' %}

            {# strings and variables translated via a filter are escaped by default #}
            {{ message|trans|raw }}
            {{ '<h3>bar</h3>'|trans|raw }}

.. tip::

    Można ustawić domenę translacyjną dla całego szablonu Twig w pojedynczym znaczniku:

    .. code-block:: jinja

           {% trans_default_domain "app" %}

    Proszę mieć na uwadze, że wpływa to tylko na bieżący szablon a nie na każdy
    "zawarty" szablon (celem uniknięcia efektów ubocznych).

.. index::
   single: szablony PHP; tłumaczenia

Szablony PHP
~~~~~~~~~~~~

Usługa translacyjna jest dostępna w szablonie PHP przy zastosowaniu helpera 
``translator``:

.. code-block:: html+php
   :linenos:

    <?php echo $view['translator']->trans('Symfony2 is great') ?>

    <?php echo $view['translator']->transChoice(
        '{0} There is no apples|{1} There is one apple|]1,Inf[ There are %count% apples',
        10,
        array('%count%' => 10)
    ) ?>
    
.. _book-translation-resource-locations:

Nazwy zasobów (plików) translacyjnych i ich lokalizacja
-------------------------------------------------------

Symfony2 wyszukuje pliki komunikatów (czyli tłumaczenia) w następujących katalogach:

* ``app/Resources/translations``;

* ``app/Resources/<bundle name>/translations``;

* ``Resources/translations/`` wewnątrz pakietu.

Miejsca te są wymienione tutaj w kolejności najwyższego priorytetu. Oznacza to,
że można przesłonić komunikaty translacyjne pakietu, umieszczając przesłaniający
plik tłumaczeń w dowolnym z dwu poprzednich katalogów.

Mechanizm przesłaniania działaj na poziomie kluczy. Oznacza to, że w pliku tłumaczeń
o najwyższym priorytecie musi się wymienić tylko przesłaniane klucze. Gdy klucz
nie zostaje znaleziony w podstawowym pliku tłumaczeń, translator automatycznie
przechodzi do plików tłumaczeń niższego poziomu.

Ważna jest też nazwa plików tłumaczeń – każdy plik tłumaczeń musi nosić nazwę zgodną
ze wzorcem: ``domena.identyfikator.typ``:

* **domena**: opcjonalny spsób organizowania komunikatów w grupy (np. ``admin``,
  ``navigation`` lub domyślnie ``messages``) - zobacz :ref:`using-message-domains`;

* **identyfikator**: oznaczenie językowe (narodowe) dla tłumaczenia
  (np. ``en_GB``, ``en``, ``pl_PL``, ``pl`` itp.);

* **typ**: typ pliku tłumaczenia (np. ``xliff``, ``php``, ``yml``).

Typ może mieć nazwę każdego zarejestrowanego loadera. Domyślnie, Symfony
dostarcza kilka loaderów, w tym:

* ``xliff``: plik XLIFF;
* ``php``: plik PHP;
* ``yml``: plik YAML.

Wybór loadera zależy tylko od programisty i jest to
tylko kwestia przyzwyczajenia. W celu poznania więcej opcji przeczytaj
:ref:`component-translator-message-catalogs`.

.. note::

    Można również zapisać tłumaczenia w bazie danych lub innym miejscu, dostarczając
    własna klasę implementującą interfejs
    :class:`Symfony\\Component\\Translation\\Loader\\LoaderInterface`.
    W celu poznania szczegółów zobacz :ref:`dic-tags-translation-loader`.

.. caution::

    Za każdym razem, gdy tworzy się *nowe* zasoby translacyjne, trzeba wyczyścić
    pamięć podręczną, tak aby Symfony mogło wykryć nowe zasoby translacyjne:

    .. code-block:: bash

        $ php app/console cache:clear

.. _book-translation-fallback:

Awaryjny identyfikator narodowy
-------------------------------

Proszę sobie wyobrazić, że ustawienie narodowe użytkownika, to ``pl_PL`` oraz że
mamy klucz translacyjny ``Symfony2 is great``. W celu znalezienia polskiego tłumaczenia,
Symfony w rzeczywistości sprawdza zasoby dla kilku różnych identyfikatorów narodowych:

1. Najpierw, Symfony przeszukuje polskie zasoby translacyjnego ``pl_PL``
   (np. ``messages.pl_PL.xliff``), kolejno w trzech katalogach translacyjnych;

2. Jeśli nie znaleziono tego tłumaczenia, Symfony przeszukije zasoby ``pl``
   (np. ``messages.pl.xliff``);

3. Jeśli tłumaczenie dalej nie zostało znalezione, Symfony używa parametru
   konfiguracyjnego ``fallback``, którego domyśłna wartość , to ``en``.
   

.. _book-translation-user-locale:

Obsługa ustawień narodowych użytkownika
---------------------------------------

Identyfikator narodowy użytkownika jest przechowywany w żądaniu i jest dostępny
poprzez obiekt ``request``::

    use Symfony\Component\HttpFoundation\Request;

    public function indexAction(Request $request)
    {
        $locale = $request->getLocale();

        $request->setLocale('en_US');
    }

.. tip::

    Przczytaj :doc:`/cookbook/session/locale_sticky_session`, aby dowiedzieć się
    jak identyfikator narodowy jest przechowywany w sesji.

.. index::
   single: tłumaczenia; rezerwowe i domyślne identyfikatory narodowe

Prosze przecztać rozdział :ref:`book-translation-locale-url` traktujacy o ustawianiu
identyfikatora narodowego użytkownika poprzez trasowanie.

.. _book-translation-locale-url:

Ustawienie narodowe a URL
~~~~~~~~~~~~~~~~~~~~~~~~~

Ponieważ można przechowywać identyfikator narodowy użytkownika w sesji, może być
kuszące użycie tego samego adresu URL do wyświetlenia zasobu w wielu różnych
językach w oparciu o ustawienie narodowe użytkownika. Na przykład,
``http://www.example.com/contact`` może pokazywać treść w języku angielskim dla
jednego użytkownika a w języku polskim dla innego użytkownika. Niestety, narusza
to podstawową zasadę internetu, że określony adres URL zwraca ten sam zasób,
niezależnie od użytkownika. Następnym problemem jest to, która wersja treści
będzie indeksowana  przez wyszukiwarki?

Lepszym rozwiązaniem jest włączenie identyfikatora narodowego do adresu URL.
Jest to w pełni obsługiwane przez specjalny parametr ``_locale``:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        contact:
            path:     /{_locale}/contact
            defaults: { _controller: AcmeDemoBundle:Contact:index }
            requirements:
                _locale: en|fr|de

    .. code-block:: xml
       :linenos:

        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="contact" path="/{_locale}/contact">
                <default key="_controller">AcmeDemoBundle:Contact:index</default>
                <requirement key="_locale">en|fr|de</requirement>
            </route>
        </routes>

    .. code-block:: php
       :linenos:

        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('contact', new Route(
            '/{_locale}/contact',
            array(
                '_controller' => 'AcmeDemoBundle:Contact:index',
            ),
            array(
                '_locale'     => 'en|pl|de',
            )
        ));

        return $collection;

Podczas stosowania specjalnegp parametru ``_locale`` w trasie, dopasowanie
identyfikatora narodowego będzie *automatycznie ustawiane w żądaniu* i może zostać
pobrane poprzez metodę :method:`Symfony\\Component\\HttpFoundation\\Request::getLocale`.
Innymi słowami, jeśli użytkownik odwiedzi URI ``/pl/contact``, to symbol ``pl``
zostanie automatycznie ustawiony jako identyfikator narodowy dla bieżącego żądania.

Można teraz w swojej aplikacji używać identyfikatora narodowego do tworzenia tras
dla innych stron tłumaczeń.

Ustawienie domyślnego identyfikatora narodowego
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Co, jeśli ustawienie narodowe użytkownika nie zostanie ustalone? Można zagwarantować,
że identyfikator narodowy jest ustawiany domyślnie przy każdym żądaniu użytkownika,
definiując klucz ``default_locale`` w opcji ``framework``:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config.yml
        framework:
            default_locale: en

    .. code-block:: xml
       :linenos:

        <!-- app/config/config.xml -->
        <framework:config>
            <framework:default-locale>en</framework:default-locale>
        </framework:config>

    .. code-block:: php
       :linenos:

        // app/config/config.php
        $container->loadFromExtension('framework', array(
            'default_locale' => 'en',
        ));

.. _book-translation-constraint-messages:

Tłumaczenie komunikatów ograniczeń
----------------------------------

Jeśli używa się ograniczeń walidacyjnych we frameworku formularzy, można w sposób
łatwy tłumaczyć komunikaty błędów. Wystarczy utworzyć zasób dla :ref:`domeny
<using-message-domains>` ``validators``.

W celu rozpoczęcia załóżmy, że tworzymy zwykły stary obiekt PHP, który trzeba
używać gdzieś w aplikacji::

    // src/Acme/BlogBundle/Entity/Author.php
    namespace Acme\BlogBundle\Entity;

    class Author
    {
        public $name;
    }

Dodamy ograniczenia do jakiejś obsługiwanej metody i ustawimy opcję komunikatu dla
tekstu zasobu translacyjnego. Na przykład, aby zagwarantować, że właściwość $name
nie jest pusta, dodajmy następujący kod:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # src/Acme/BlogBundle/Resources/config/validation.yml
        Acme\BlogBundle\Entity\Author:
            properties:
                name:
                    - NotBlank: { message: "author.name.not_blank" }

    .. code-block:: php-annotations
       :linenos:

        // src/Acme/BlogBundle/Entity/Author.php
        use Symfony\Component\Validator\Constraints as Assert;

        class Author
        {
            /**
             * @Assert\NotBlank(message = "author.name.not_blank")
             */
            public $name;
        }

    .. code-block:: xml
       :linenos:

        <!-- src/Acme/BlogBundle/Resources/config/validation.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <constraint-mapping xmlns="http://symfony.com/schema/dic/constraint-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/constraint-mapping http://symfony.com/schema/dic/constraint-mapping/constraint-mapping-1.0.xsd">

            <class name="Acme\BlogBundle\Entity\Author">
                <property name="name">
                    <constraint name="NotBlank">
                        <option name="message">author.name.not_blank</option>
                    </constraint>
                </property>
            </class>
        </constraint-mapping>

    .. code-block:: php
       :linenos:

        // src/Acme/BlogBundle/Entity/Author.php

        // ...
        use Symfony\Component\Validator\Mapping\ClassMetadata;
        use Symfony\Component\Validator\Constraints\NotBlank;

        class Author
        {
            public $name;

            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                $metadata->addPropertyConstraint('name', new NotBlank(array(
                    'message' => 'author.name.not_blank',
                )));
            }
        }

Utworzymy teraz plik tłumaczeń w katalogu ``validators`` dla komunikatów ograniczeń.
Katalog ``validators`` zwykle tworzy się w katalogu ``Resources/translations/``
pakietu.

.. configuration-block::

    .. code-block:: xml
       :linenos:

        <!-- validators.en.xliff -->
        <?xml version="1.0"?>
        <xliff version="1.2" xmlns="urn:oasis:names:tc:xliff:document:1.2">
            <file source-language="en" datatype="plaintext" original="file.ext">
                <body>
                    <trans-unit id="1">
                        <source>author.name.not_blank</source>
                        <target>Please enter an author name.</target>
                    </trans-unit>
                </body>
            </file>
        </xliff>

    .. code-block:: php
       :linenos:

        // validators.en.php
        return array(
            'author.name.not_blank' => 'Please enter an author name.',
        );

    .. code-block:: yaml
       :linenos:

        # validators.en.yml
        author.name.not_blank: Please enter an author name.

Tłumaczenie zawartości bazydanych
---------------------------------

Tłumaczenie treści z bazy danych powinno być obsługiwane przez rozszrzenie
`Translatable Extension`_ Doctrine. Więcej informacji znajdziesz w dokumentacji
tej biblioteki.


Podsumowanie
------------

Z komponentem Symfony2 Translation, tworzenie umiędzynarodowionych aplikacji nie
musi być bolesnym procesem i sprowadza się do kilku prostych kroków:

* Uabstrakcyjnienie komunikatów w aplikacji przez owinięcie każdego z nich metodą 
  :method:`Symfony\\Component\\Translation\\Translator::trans` lub
  :method:`Symfony\\Component\\Translation\\Translator::transChoice` methods
  (learn about this in :doc:`/components/translation/usage`);
  
* Przetłumaczenie każdego komunikatu dla wielu ustawień narodowych przez utworzenie
  plików tłumaczeń komunikatów. Symfony2 odnajduje i przetwarza każdy plik ponieważ
  jego nazwa zgodna jest z określoną konwencją;

* Zarządzanie ustawieniami narodowymi, których oznaczenia (identyfikatory) są
  przechowywane w żądaniu, ale mogą również być ustawione w sesji użytkownika.

.. _`i18n`: http://en.wikipedia.org/wiki/Internationalization_and_localization
.. _`ISO 3166-1 alpha-2`: http://en.wikipedia.org/wiki/ISO_3166-1#Current_codes
.. _`ISO 639-1`: http://en.wikipedia.org/wiki/List_of_ISO_639-1_codes
.. _`Translatable Extension`: https://github.com/l3pp4rd/DoctrineExtensions
 