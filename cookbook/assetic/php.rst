.. index::
    double Assetic; Bootstrap
    single: Assetic; biblioteki PHP

Biblioteki PHP a łączenie, kompilowanie i minimalizowanie aktywów internetowych
===============================================================================

Oficjalnie w "Najlepszych praktykach Symfony" zaleca sie używanie Assetic do
:doc:`zarządzania aktywami internetowymi </best_practices/web-assets>`, chyba że
wykorzystuje się jakieś wygodne narzędzie "front-endowe" oparte na JavaScript.

Nawet jeśli rozwiązanie oparte o JavaScript sa najbardziej odpowiednie z technicznego
punktu widzenia, to użycie alternatywnych bibliotek w czystym PHP może być przydatne
w kilku przypadkach:

* Jeśli nie można zainstalować lub użyć ``npm`` i innych rozwiazań JavaScript;
* Jeśli woli się ograniczyć ilość różnych technologii używanych w swojej aplikacji;
* Jeśli chce sie uprościć wdrażanie aplikacji.

W tym artykule dowiesz się jak połączyć i zminimalizowac pliki CSS i JavaScript
i jak skompilować pliki Sass używając w Assetic bibliotek PHP.

Instalowanie zewnętrznych bibliotek kompresowania
-------------------------------------------------

Assetic zawiera wiele gotowych do użycia filtrów, ale nie zawiera powiazanych z nimi
bibliotek. Dlatego, przed udostępnieniem filtrów opisanych w tym artykule musi się
zainstalować dwie biblioteki. Prosze otworzyć konsolę poleceń, skonfigurować się
w katalogu głównym projektu i wykonać następujące polecenia:

.. code-block:: bash

    $ composer require leafo/scssphp
    $ composer require patchwork/jsqueeze:"~1.0"

To bardzo ważne, aby zachować ograniczenie wersji ``~1.0`` dla zależności ``jsqueeze``,
ponieważ najnowsze stabilne wersje nie są kompatybilne z Assetic.

Organizowanie plików aktywów internetowych
------------------------------------------

Ten przykład będzie zawierać konfigurację stosujaca framework Bootstrap CSS, jQuery,
FontAwesome i kilka zwykłych plików CSS i JavaScript aplikacji (o nazwach ``main.css``
i ``main.js``). Zalecana struktura katalogowa jest pokazana ponizej:

.. code-block:: text

    web/assets/
    ├── css
    │   ├── main.css
    │   └── code-highlight.css
    ├── js
    │   ├── bootstrap.js
    │   ├── jquery.js
    │   └── main.js
    └── scss
        ├── bootstrap
        │   ├── _alerts.scss
        │   ├── ...
        │   ├── _variables.scss
        │   ├── _wells.scss
        │   └── mixins
        │       ├── _alerts.scss
        │       ├── ...
        │       └── _vendor-prefixes.scss
        ├── bootstrap.scss
        ├── font-awesome
        │   ├── _animated.scss
        │   ├── ...
        │   └── _variables.scss
        └── font-awesome.scss

Łączenie i minimalizowanie plików CSS oraz kompilowanie plików SCSS
-------------------------------------------------------------------

W pierwszej kolejności trzeba skonfigurować nowy filtr ``scssphp`` Assetic:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        assetic:
            filters:
                scssphp:
                    formatter: 'Leafo\ScssPhp\Formatter\Compressed'
                # ...

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" charset="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:assetic="http://symfony.com/schema/dic/assetic">

            <assetic:config>
                <filter name="scssphp" formatter="Leafo\ScssPhp\Formatter\Compressed" />
                <!-- ... -->
            </assetic:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('assetic', array(
            'filters' => array(
                 'scssphp' => array(
                     'formatter' => 'Leafo\ScssPhp\Formatter\Compressed',
                 ),
                 // ...
            ),
        ));

Wartość opcji ``formatter`` jest w pełni kwalifikowana nazwą klasy formattera,
uzywaną przez filtr do produkcji skompilowanych plików CSS. Używając formatera
kompresowania bedzie mozna zminimalizować pik wyjściowy, niezaleznie od tego, czy
pliki sa zwykłymi plikami CSS czy plikami SCSS.

Następnie trzeba przerobić szablon Twig, doając znacznik ``{% stylesheets %}``
Assetic:

.. code-block:: html+jinja

    {# app/Resources/views/base.html.twig #}
    <!DOCTYPE html>
    <html>
        <head>
            <!-- ... -->

            {% stylesheets filter="scssphp" output="css/app.css"
                "assets/scss/bootstrap.scss"
                "assets/scss/font-awesome.scss"
                "assets/css/*.css"
            %}
                <link rel="stylesheet" href="{{ asset_url }}" />
            {% endstylesheets %}

Ta prosta konfiguracja kompiluje, łączy pliki SCSS w jeden zminifikowany plik
CSS, który zostale umieszczony w ``web/css/app.css``. Jest to tylko plik CSS,
który bedzie serwowany do odwiedzających.

Łączenie i minimalizowanie plików JavaScript
--------------------------------------------

Najpierw trzeba skonfigurować nowy filtr ``jsqueeze`` Assetic, w następujacy
sposób:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        assetic:
            filters:
                jsqueeze: ~
                # ...

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" charset="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:assetic="http://symfony.com/schema/dic/assetic">

            <assetic:config>
                <filter name="jsqueeze" />
                <!-- ... -->
            </assetic:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('assetic', array(
            'filters' => array(
                 'jsqueeze' => null,
                 // ...
            ),
        ));

Następnie trzeba poprawić kod szablonu Twig, dodając znacznik ``{% javascripts %}``
Assetic:

.. code-block:: html+jinja

    <!-- ... -->

        {% javascripts filter="?jsqueeze" output="js/app.js"
            "assets/js/jquery.js"
            "assets/js/bootstrap.js"
            "assets/js/main.js"
        %}
            <script src="{{ asset_url }}"></script>
        {% endjavascripts %}

        </body>
    </html>

Konfiguracja ta scala wszystkie pliki JavaScript, minimalizuje zawartość i zapisuje
wyjscie w postaci pliku ``web/js/app.js``, który jako jedyny jest serwowany
odwiedzającym.

Poprzedzenie znakiem ``?`` nazwy filtra ``jsqueeze`` powiadomii Assetic, aby
nie stosował filtra w trybie debugowania aplikacji. W praktyce oznacza to, że
przy projektowaniu ogląda się pliki niezminifikowane a w środowisku produkcyjnym
pliki zostaja scalone do jednego, zminifikowanego pliku.
