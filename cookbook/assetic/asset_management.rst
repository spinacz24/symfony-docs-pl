.. index::
   single: Assetic; wprowadzenie

Jak zastosować Assetic do zarządzania aktywami
==============================================

Assetic łączy w sobie dwie podstawowe idee: :ref:`aktywów<cookbook-assetic-assets>`
i :ref:`filtrów<cookbook-assetic-filters>`. Pliki CSS, JavaScript i obrazy, to zasoby.
Zasoby po załadowaniu i ewentualnym przetworzeniu stają się aktywami. Filtry to coś,
co może być zastosowane na aktywach zanim te zostaną przesłane do przeglądarki.
Pozwala to odseparować pliki zasobów trzymanych w aplikacji od plików rzeczywiście
prezentowanych użytkownikowi.

Bez Assetic, pliki obecne w aplikacji są serwowane bezpośrednio:

.. configuration-block::

    .. code-block:: html+jinja

        <script src="{{ asset('js/script.js') }}" type="text/javascript" />

    .. code-block:: php

        <script src="<?php echo $view['assets']->getUrl('js/script.js') ?>" type="text/javascript" />

Jednakże dzięki Assetic można manipulować aktywami na różne sposoby (albo załadować
je z dowolnego miejsca) zanim te zostaną wyświetlone użytkownikowi. Oznacza to, że można:

* Skompresować i połączyć wszystkie pliki CSS i JS

* Przepuścić wszystkie, albo tylko część plików CSS i JS przez jakiś kompilator,
  taki jak LESS, SASS albo CoffeeScript

* Zoptymalizować obraz na zdjęciach

.. _cookbook-assetic-assets:

Aktywa
------

Korzystanie z Assetic posiada wiele zalet nad metodą bezpośredniego serwowania
plików. Pliki nie muszą być przechowywane w miejscach z których są serwowane oraz
mogą zostać pobrane z różnych źródeł, takich jak pakiety.

Można używać Assetic zarówno dla :ref:`stylów CSS <cookbook-assetic-including-css>`
jak i :ref:`plików JavaScript<cookbook-assetic-including-javascript>` oraz
:ref:`obrazów <cookbook-assetic-including-image>`. Idea przewodnia
dodawania takich plików jest w zasadzie taka sama, ale z nieco inną składnią.
Assetic wykorzystuje własne znaczniki aktywów systemu Twig, takie jak:

* {% javascripts .... %}
* {% stylesheets ... %}
* {% image .. %}

oraz helpery PHP: ``javascripts``, ``stylesheets`` i ``images``. 

.. _cookbook-assetic-including-javascript:

Dołączanie plików JavaScript
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Dla dołączenia plików JavaScript trzeba w szablonie zastosować znacznik ``javascripts`:

.. configuration-block::

    .. code-block:: html+jinja

        {% javascripts '@AppBundle/Resources/public/js/*' %}
            <script src="{{ asset_url }}"></script>
        {% endjavascripts %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->javascripts(
            array('@AppBundle/Resources/public/js/*')
        ) as $url): ?>
            <script src="<?php echo $view->escape($url) ?>"></script>
        <?php endforeach ?>

.. note::

    Jeśli w szablonie aplikacji wykorzystujesz domyślne nazwy bloków z Symfony
    Standard Edition, to blok skryptów JavaScript ma najczęściej nazwę ``javascripts``
    i to w nim trzeba umieszczać znacznik ``javascripts``:

    .. code-block:: html+jinja

        {# ... #}
        {% block javascripts %}
            {% javascripts '@AppBundle/Resources/public/js/*' %}
                <script src="{{ asset_url }}"></script>
            {% endjavascripts %}
        {% endblock %}
        {# ... #}

.. tip::

    Można również dołączyć style CSS: :ref:`cookbook-assetic-including-css`.
    
W tym przykładzie wszystkie pliki w katalogu ``Resources/public/js/`` z `AppBundle
zostaną wczytane i zaserwowane z innych lokalizacji. Rzeczywisty znacznik mógłby
wyglądać na przykład tak:
    
.. code-block:: html

    <script src="/app_dev.php/js/abcd123.js"></script>

Jest to punkt kluczowy - gdy pozwolisz Assetic obsługiwać swoje aktywa, będą one
serwowane z różnych lokalizacji. *Będzie* to powodować problemy z plikami CSS,
które odwołują się do obrazów poprzez ścieżki względne. Zobacz :ref:`cookbook-assetic-cssrewrite`.

.. _cookbook-assetic-including-css:

Dołączanie stylów CSS
~~~~~~~~~~~~~~~~~~~~~

Dla dołączenia plików CSS, można użyć tej samej metody co powyżej stosując znacznik
``stylesheets``:

.. configuration-block::

    .. code-block:: html+jinja

        {% stylesheets 'bundles/app/css/*' filter='cssrewrite' %}
            <link rel="stylesheet" href="{{ asset_url }}" />
        {% endstylesheets %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->stylesheets(
            array('bundles/app/css/*'),
            array('cssrewrite')
        ) as $url): ?>
            <link rel="stylesheet" href="<?php echo $view->escape($url) ?>" />
        <?php endforeach ?>

.. note::

    Jeśli w szablonie aplikacji wykorzystujesz domyślne nazwy bloków z Symfony
    Standard Edition, to blok skryptów JavaScript ma najczęściej nazwę ``stylesheets``
    i to w nim trzeba umieszczać znacznik ``stylesheets``:

    .. code-block:: html+jinja

        {# ... #}
        {% block stylesheets %}
            {% stylesheets 'bundles/app/css/*' filter='cssrewrite' %}
                <link rel="stylesheet" href="{{ asset_url }}" />
            {% endstylesheets %}
        {% endblock %}
        {# ... #}

Z uwagi na to, że Assetic zmienia ścieżki do swoich aktywów, najprawdopodobniej
obrazy tła przestaną działać (lub inne zasoby), które używają ścieżek względnych,
chyba, że zastosowano filtr :ref:`cssrewrite<cookbook-assetic-cssrewrite>`.

.. note::

    Proszę zauważyć, że w pierwotnym przykładzie, gdzie dołączano pliki JavaScript,
    odniesiono się do nich z użyciem ``@AcmeFooBundle/Resources/public/file.js``,
    zaś w tym przykładzie odwołanno się do plików CSS poprzez rzeczywistą, publicznie
    widoczną ścieżkę: ``bundles/acme_foo/css``. Można używać obu metod, należy jednak
    pamiętać, że istnieje znany problem, który powoduje błędne działanie filtra
    ``cssrewrite`` z użyciem składni ``@AcmeFooBundle``.

.. _cookbook-assetic-including-image:

Dołaczanie obrazów
~~~~~~~~~~~~~~~~~~

Dla dołączenia obrazu trzeba wykorzystac znacznik ``image``.

.. configuration-block::

    .. code-block:: html+jinja

        {% image '@AppBundle/Resources/public/images/example.jpg' %}
            <img src="{{ asset_url }}" alt="Example" />
        {% endimage %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->image(
            array('@AppBundle/Resources/public/images/example.jpg')
        ) as $url): ?>
            <img src="<?php echo $view->escape($url) ?>" alt="Example" />
        <?php endforeach ?>

Assetic można również uzyć w celu optymalizacji obrazu. Wiecej informacji można
znaleźć w artykule :doc:`/cookbook/assetic/jpeg_optimize`.

.. tip::

    Zamiast stosowanie Assetic dla dołączania obrazów można rozważyć wykorzystanie
    społecznościowego pakietu `LiipImagineBundle`_, który umożlwia kompresowanie
    i manipulowanie obrazami (obracanie, zmiana rozmiarów, znak wodny itd.) 
    przed ich serwowaniem.

.. _cookbook-assetic-cssrewrite:

Ustalanie ścieżki w plikach CSS z użyciem filtra ``cssrewrite``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Ponieważ Assetic generuje nowe adresy URL dla aktywów, wszystkie ścieżki względne
wewnątrz plików CSS nie będa działać. By temu zaradzić, trzeba użyć filtr
``cssrewrite`` w znaczniku ``stylesheets``. Pozwala on przeanalizować pliki CSS
i skorygować wszystkie ścieżki wewnętrzne tak, by odzwierciedlały nowe położenie.

Można zobaczyć to w przykładzie z poprzedniegi rozdziału.

.. caution::
   
   Przy stosowaniu filtra ``cssrewrite``, nie powinno się odwoływać do plików CSS
   za pomocą składni ``@AppBundle``. W celu poznania szczegółow proszę się zapoznać
   z uwagą w poprzednim rozdziale.

Łączenie aktywów
~~~~~~~~~~~~~~~~

Jedną z możliwosci Assetic jest łączenie wielu plików w jeden. Pomaga to zredukować
liczbę żądań HTTP, co jest niezbędne dla wydajności części publicznej aplikacji.
Pozwala to także na sprawniejsze zarządanie plikami poprzez dzielenie ich na mniejsze,
łatwiejsze w utrzymaniu części. Może to pomóc w optymalizacji wielkości pliku
wyjściowego, bowiem pozwala oddzielić pliki specyficzne dla danego projektu od tych,
które mogą zostać użyte w innych aplikacjach, wciąż serwując je jako jeden plik:

.. configuration-block::

    .. code-block:: html+jinja

        {% javascripts
            '@AppBundle/Resources/public/js/*'
            '@AcmeBarBundle/Resources/public/js/form.js'
            '@AcmeBarBundle/Resources/public/js/calendar.js' %}
            <script src="{{ asset_url }}"></script>
        {% endjavascripts %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->javascripts(
            array(
                '@AppBundle/Resources/public/js/*',
                '@AcmeBarBundle/Resources/public/js/form.js',
                '@AcmeBarBundle/Resources/public/js/calendar.js',
            )
        ) as $url): ?>
            <script src="<?php echo $view->escape($url) ?>"></script>
        <?php endforeach ?>

W środowisku ``dev`` każdy plik jest nadal serwowany indywidualnie, tak aby można
było łatwiej debugować problemy. Jednak w środowisku ``prod`` (a dokładniej, gdy
flaga ``debug`` jest ustawiona na ``false``), wszystko zostanie wygenerowane w jednym
znaczniku ``script``, który zawierał będzie zawartość wszystkich użytych plików JavaScript.

.. tip::

    Jeśli dopiero co poznajesz Assetic i uruchamiasz aplikacje w środowisku ``prod``
    (za pomocą kontrolera ``app.php``), prawdopodobnie doświadczysz, że wszystkie
    pliki CSS i JS przestały działać. Nie martw się! Jest to celowe. Po szczegółowe
    informacje dotyczące korzystania Assetic w środowisku ``prod`` sięgnij do
    :ref:`cookbook-assetic-dumping`.

Łączenie plików odnosi się nie tylko do *własnych* plików. Można również użyć
Assetic do połączenia zasobów zewnetrznych, takich jak jQuery, z własnymi i połączyć
je w jeden plik:

.. configuration-block::

    .. code-block:: html+jinja

        {% javascripts
            '@AppBundle/Resources/public/js/thirdparty/jquery.js'
            '@AppBundle/Resources/public/js/*' %}
            <script src="{{ asset_url }}"></script>
        {% endjavascripts %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->javascripts(
            array(
                '@AppBundle/Resources/public/js/thirdparty/jquery.js',
                '@AppBundle/Resources/public/js/*',
            )
        ) as $url): ?>
            <script src="<?php echo $view->escape($url) ?>"></script>
        <?php endforeach ?>


Stosowanie nazwanych aktywów
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Dyrektywy konfiguracyjne AsseticBundle pozwalają zdefiniowac zestawy nazwanych
aktywów.
Można to zrobić definiując pliki wejściowe, filtry oraz pliki wyjściowe w konfiguracji
w sekcji ``assetic``. Więcej na ten temat mozna dowiedzieć sie w 
:doc:`Informatorze konfiguracyjnym assetic </reference/configuration/assetic>`.

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        assetic:
            assets:
                jquery_and_ui:
                    inputs:
                        - '@AppBundle/Resources/public/js/thirdparty/jquery.js'
                        - '@AppBundle/Resources/public/js/thirdparty/jquery.ui.js'

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:assetic="http://symfony.com/schema/dic/assetic">

            <assetic:config>
                <assetic:asset name="jquery_and_ui">
                    <assetic:input>@AppBundle/Resources/public/js/thirdparty/jquery.js</assetic:input>
                    <assetic:input>@AppBundle/Resources/public/js/thirdparty/jquery.ui.js</assetic:input>
                </assetic:asset>
            </assetic:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('assetic', array(
            'assets' => array(
                'jquery_and_ui' => array(
                    'inputs' => array(
                        '@AppBundle/Resources/public/js/thirdparty/jquery.js',
                        '@AppBundle/Resources/public/js/thirdparty/jquery.ui.js',
                    ),
                ),
            ),
        );

Po zdefiniowaniu nazwanych aktywów, mozna odwołac się do nich w szablonach stosując
notacje ``@named_asset``:

.. configuration-block::

    .. code-block:: html+jinja

        {% javascripts
            '@jquery_and_ui'
            '@AppBundle/Resources/public/js/*' %}
            <script src="{{ asset_url }}"></script>
        {% endjavascripts %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->javascripts(
            array(
                '@jquery_and_ui',
                '@AppBundle/Resources/public/js/*',
            )
        ) as $url): ?>
            <script src="<?php echo $view->escape($url) ?>"></script>
        <?php endforeach ?>

.. _cookbook-assetic-filters:

Filtry
------

Gdy filtry są zarządzane przez Assetic, można zastosować je do aktywów, zanim
te zostaną zaserwowane użytkownikowi. Obejmuje to filtry, które kompresują dane
wyjściowe aktywów do mniejszych rozmiarów (i poprawiają wydajność części publicznej
aplikacji).
Inne filtry mogą skompilować plik JavaScript z plików CoffeeScript albo przetworzyć
SASS w CSS. W rzeczywistości, Assetic ma dość pokaźną listę dostępnych filtrów.

Wiele z tych filtrów nie zadziała bezpośrednio, gdyż używa bibliotek zewnetrznych
do wykonywania najcięższej, algorytmicznej pracy. Oznacza to, że nieraz będzie trzeba
najpierw zainstalować takie biblioteki, aby potem użyć konkretnego filtru. Zaletą 
korzystania z Assetic do wywoływania tych bibliotek (w przeciwieństwie do korzystania
z nich bezpośrednio) jest to, że zamiast uruchamiać je ręcznie podczas pracy, Assetic
zadba o to za nas i usunie ten krok z procesu tworzenia i wdrażania aplikacji.

W celi użycia filtru, trzeba najpierw określić go w konfiguracji Assetic. Dodawanie
filtru tutaj nie znaczy, że jest już używany - to po prostu oznacza, że jest on
możliwy do wykorzystania (można skorzystać z ponizszego filtra).

Na przykład, aby użyć JavaScript YUI Compressor, powinna zostać dodana następująca
konfiguracja:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        assetic:
            filters:
                uglifyjs2:
                    bin: /usr/local/bin/uglifyjs

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <assetic:config>
            <assetic:filter
                name="uglifyjs2"
                bin="/usr/local/bin/uglifyjs" />
        </assetic:config>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('assetic', array(
            'filters' => array(
                'uglifyjs2' => array(
                    'bin' => '/usr/local/bin/uglifyjs',
                ),
            ),
        ));

Teraz, aby rzeczywiście *użyć* filtru na grupie plików JavaScript, wystarczy następująco
zmodyfikować plik szablonu:

.. configuration-block::

    .. code-block:: html+jinja

        {% javascripts '@AppBundle/Resources/public/js/*' filter='uglifyjs2' %}
            <script src="{{ asset_url }}"></script>
        {% endjavascripts %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->javascripts(
            array('@AppBundle/Resources/public/js/*'),
            array('uglifyjs2')
        ) as $url): ?>
            <script src="<?php echo $view->escape($url) ?>"></script>
        <?php endforeach ?>

Bardziej szczegółowy przewodnik na temat konfiguracji i korzystania z filtrów Assetic,
jak również informacji o trybie debugowania Assetic można znaleźć w :doc:`/cookbook/assetic/yuicompressor`.

Kontrolowanie używanych adresów URL
-----------------------------------

Jeśli chcesz, możesz kontrolować adresy URL generowane przez Assetic. Są one tworzone
z szablonu i względne w stosunku do głównego dokumentu publicznego:

.. configuration-block::

    .. code-block:: html+jinja

        {% javascripts '@AppBundle/Resources/public/js/*' output='js/compiled/main.js' %}
            <script src="{{ asset_url }}"></script>
        {% endjavascripts %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->javascripts(
            array('@AppBundle/Resources/public/js/*'),
            array(),
            array('output' => 'js/compiled/main.js')
        ) as $url): ?>
            <script src="<?php echo $view->escape($url) ?>"></script>
        <?php endforeach ?>

.. note::

    Symfony zawiera metody do generowania pamięci podręcznej typu *cache-busting*,
    dla których ostateczny adres URL generowany przez Assetic zawiera parametr zapytania,
    który może być zwiększany poprzez konfigurację przy każdym rozmieszczeniu aktywa.
    Aby uzyskać więcej informacji, zapoznaj się z opcją konfiguracji :ref:`ref-framework-assets-version`.

.. _cookbook-assetic-dumping:

Zrzut plików aktywów
--------------------

W środowisku ``dev``, Assetic generuje ścieżki do plików CSS i JavaScript, które
fizycznie nie istnieją na komputerze. Ścieżki są tak czy inaczej generowane, gdyż
wewnętrzny kontroler Symfony jest w stanie otworzyć pliki, by zaserwować ich zawartość
(zaraz po uruchomieniu filtrów).

Ten rodzaj dynamicznego serwowania przetworzonych aktywów daje dużo korzyści, gdyż
oznacza to, że można od razu zobaczyć stan wszystkich plików aktywów, które uległy
zmianie. Z drugiej strony, może przynieść i straty z uwagi na spowolnienie aplikacji.
Jeśli używa się zbyt wielu filtrów, może okazać się to wręcz frustrujące.

Na szczęście Assetic zapewnia możliwość zrzutu aktywów do rzeczywistych plików,
zamiast generowania ich dynamicznie.


Zrzut plików aktywów w środowisku ``prod``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

W środowisku ``prod``, pliki JS i CSS sa reprezentowane przez pojedynczy znacznik.
Innymi słowy, zamiast widzieć każdy plik JavaScript, który załączono w źródle,
nieraz zobaczy się coś takiego:

.. code-block:: html

    <script src="/app_dev.php/js/abcd123.js"></script>

Co więcej, plik ten w rzeczywistości **nie** istnieje, ani nie jest również dynamicznie
generowany przez Symfony (jak pliki aktywów w środowisku ``dev``). Jest to celowe -
pozwolenie Symfony na generowanie tych plików dynamicznie w środowisku produkcyjnym
byłoby po prostu zbyt wolne.

Zamiast tego, za każdym razem gdy korzysta się ze środowiska ``prod`` (a zatem za
każdym razem gdy następuje proces wdrażania), powinno sie uruchomiać następujące zadanie:

.. code-block:: bash

    $ php app/console assetic:dump --env=prod --no-debug

To spowoduje fizyczne generowanie każdego pliku, który jest potrzebny, (np. ``/js/abcd123.js``).
W przypadku aktualizacji aktywów, trzeba uruchomić to zadanie ponownie i wygenerować
jeszcze raz te pliki.

Zrzut plików aktywów w środowisku ``dev``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Domyślnie, każda ścieżka aktywa generowana w środowisku ``dev`` jest obsługiwana
dynamicznie przez Symfony. Nie ma to wad (zmiany widać natychmiast), z zastrzeżeniem,
że aktywa mogą ładować się zauważalnie wolniej. Jeśli uważasz, że aktywa wczytują się
zbyt wolno, skorzystaj z tej instrukcji.

Po pierwsze, poinformuj Symfony aby zatrzymać dynamicznie przetwarzanie tych plików.
Wprowadź następującą zmianę w pliku konfiguracji ``config_dev.yml``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config_dev.yml
        assetic:
            use_controller: false

    .. code-block:: xml

        <!-- app/config/config_dev.xml -->
        <assetic:config use-controller="false" />

    .. code-block:: php

        // app/config/config_dev.php
        $container->loadFromExtension('assetic', array(
            'use_controller' => false,
        ));

Następnie, ponieważ Symfony nie jest już odpowiedzialne za generowanie aktywów,
trzeba zrzucić je ręcznie. Aby to zrobić, wykonaj następujące czynności:

.. code-block:: bash

    $ php app/console assetic:dump

To polecenie fizycznie zapisuje wszystkie pliki aktywów w środowisku ``dev``.
Dużą wadą jest, że trzeba uruchamiać je za każdym razem gdy zaktualizowano aktywa.
Na szczęście, przekazując opcje ``--watch`` umożliwi się automatycznie ich
przegenerowywanie  *w chwili ich zmiany*:

.. code-block:: bash

    $ php app/console assetic:dump --watch

Ponieważ uruchomienie tego polecenia w środowisku ``dev`` może wygererować dość
sporo plików, zazwyczaj dobrym pomysłem dla tak generowanych plików jest wskazanie
dla nich odrębnego katalogu (np. ``/js/compiled``), tak aby utrzymać wszystko
w sposób zorganizowany:

.. configuration-block::

    .. code-block:: html+jinja

        {% javascripts '@AppBundle/Resources/public/js/*' output='js/compiled/main.js' %}
            <script src="{{ asset_url }}"></script>
        {% endjavascripts %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->javascripts(
            array('@AppBundle/Resources/public/js/*'),
            array(),
            array('output' => 'js/compiled/main.js')
        ) as $url): ?>
            <script src="<?php echo $view->escape($url) ?>"></script>
        <?php endforeach ?>

.. _`LiipImagineBundle`: https://github.com/liip/LiipImagineBundle
