.. index::
   single: Assetic; UglifyJS

Jak minifikować pliki CSS i JS (używając UglifyJS i UglifyCSS)
==============================================================

`UglifyJS`_ to zestaw narzędzi JavaScript do parsowania, kompresowania i upiększania
skryptów JS. Może zostać użyte do łączenia i minifikowania aktywów JavaScript, co
dzięki czemu zmniejsza się ilość żądań HTTP i przyśpiesza się ładowanie strony.
`UglifyCSS`_ jest narzędziem do kompresowania i upiekszania skryptów CSS i jest
bardzo podobne do UglifyJS.

W tym artykule omawia się szczegółowo instalację, konfigurację i stosowanie UglifyJS.
UglifyCSS działa prawie tak samo, więc omawiamy to krótko.

Instalacja UglifyJS
-------------------

UglifyJS jest dostępne jako moduł `Node.js`_. W pierwszej kolejności trzeba
`zainstalować Node.js`_ i następnie zdecydować się na globalną albo lokalną
instalację UglifyJS.

Instalacja globalna
~~~~~~~~~~~~~~~~~~~

Instalacja globalna sprawia, ze wszystkie projekty uzywać beda tej samej wersji
UglifyJS, co jest łatwiejsze w utrzymaniu. Proszę otworzyc konsolę poleceń
i wykonać następujące polecenia (jako użytkownik root):

.. code-block:: bash

    $ npm install -g uglify-js

Teraz będzie można wykonywać polecenie ``uglifyjs`` gdziekolwiek w swoim systemie:

.. code-block:: bash

    $ uglifyjs --help

Instalacja lokalna
~~~~~~~~~~~~~~~~~~

Możliwe jest zainstalowanie UglifyJS tylko wewnątrz projektu, co jest przydatne
gdy projekt wymaga specyficznej wersji UglifyJS. W celu wykonania tego, trzeba
użyć wyżej podane polecenie, będąc skonfigurowanym w katalogu projektu, ale bez
opcji ``-g``:

.. code-block:: bash

    $ cd /path/to/your/symfony/project
    $ npm install uglify-js --prefix app/Resources

Zaleca sie zainstalowanie UglifyJS w folderze ``app/Resources`` i dodanie folderu
``node_modules`` do kontroli wersji. Ewentualnie można utworzyć plik `package.json`_
npm i w nim określić zależności.

Teraz można wykonywać polecenie ``uglifyjs`` w katalogu ``node_modules``:

.. code-block:: bash

    $ "./app/Resources/node_modules/.bin/uglifyjs" --help

Konfiguracja filtra ``uglifyjs2``
---------------------------------

Teraz trzeba skonfigurować Symfony do wykorzystywania filtra ``uglifyjs2`` podczas
przetwarzania swoicj skryptów JavaScript:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        assetic:
            filters:
                uglifyjs2:
                    # the path to the uglifyjs executable
                    bin: /usr/local/bin/uglifyjs

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <assetic:config>
            <!-- bin: the path to the uglifyjs executable -->
            <assetic:filter
                name="uglifyjs2"
                bin="/usr/local/bin/uglifyjs" />
        </assetic:config>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('assetic', array(
            'filters' => array(
                'uglifyjs2' => array(
                    // the path to the uglifyjs executable
                    'bin' => '/usr/local/bin/uglifyjs',
                ),
            ),
        ));

.. note::

    Ścieżka do miejsca, gdzie zainstalowane jest UglifyJS zależy od systemu.
    Dla odnalezienia, gdzie npm przechowuje folder ``bin`` trzeba wykonać
    następujące polecenie:

    .. code-block:: bash

        $ npm bin -g

    Wyprowadzi to nazwę (ścieżkę) folderu w systmie plików, wewnątrz którego
    powinien się znajdować plik wykonywalny UglifyJS.

    Jeśli zainstalowało się UglifyJS lokalnie, można znaleźć folder ``bin`` wewnątrz
    folderu ``node_modules``. W tym przypadku jest to folder ``.bin``.

Teraz można już uzyskać dostęp do filtra ``uglifyjs2`` w swojej aplikacji.

Konfiguracja binariów ``node``
------------------------------

Zwykle Assetic próbuje automatycznie odnaleźć binaria *node*. Jeśli nie może to
zrobić, trzeba skonfigurować lokalizację w kluczu ``node``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        assetic:
            # the path to the node executable
            node: /usr/bin/nodejs
            filters:
                uglifyjs2:
                    # the path to the uglifyjs executable
                    bin: /usr/local/bin/uglifyjs

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <assetic:config
            node="/usr/bin/nodejs" >
            <assetic:filter
                name="uglifyjs2"
                bin="/usr/local/bin/uglifyjs" />
        </assetic:config>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('assetic', array(
            'node' => '/usr/bin/nodejs',
            'uglifyjs2' => array(
                    // the path to the uglifyjs executable
                    'bin' => '/usr/local/bin/uglifyjs',
                ),
        ));

Minifikacja aktywów
-------------------

W celu zastosowania UglifyJS na aktywach, trzeba dodać opcje ``filter`` z znaczniku
asset szablonu, aby powiadomić Assetic by stosował filtr ``uglifyjs2``:

.. configuration-block::

    .. code-block:: html+jinja

        {% javascripts '@AppBundle/Resources/public/js/*' filter='uglifyjs2' %}
            <script src="{{ asset_url }}"></script>
        {% endjavascripts %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->javascripts(
            array('@AppBundle/Resources/public/js/*'),
            array('uglifyj2s')
        ) as $url): ?>
            <script src="<?php echo $view->escape($url) ?>"></script>
        <?php endforeach ?>

.. note::

    Powyższy przykład zakłada, że masz pakiet o nazwie AppBundle i pliki
    JavaScript znajduja sie w katalogi ``Resources/public/js`` pakietu.
    Pliki JavaScript można umieścic gdziekolwiek w projekcie i wystarczy tylko
    podać własciwą scieżkę.

Po dodaniu filtra ``uglifyjs2`` do powyższego znacznika aktywów, powinno sie teraz
zobaczyć zminifikowany plik JavaScript, ładowany znacznie szybciej.

Wyłączanie minifikacji w trybie debugowania
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Zminifikowane pliki JavaScript sa bardzo trudne do czytania i tym bardziej do
debugowania. Z tego powodu Assetic pozwala na wyłaczenie określonego filtra w
trybie debugowania (np. dla ``app_dev.php``). Można to zrobić poprzedzając
nazwę fitra w szablonie znakiem zapytania: ``?``. Powiadamia to Assetic aby tylko
stosował ten filtr, gdy tryb debugowania jest wyłączony (np. dla ``app.php``):

.. configuration-block::

    .. code-block:: html+jinja

        {% javascripts '@AppBundle/Resources/public/js/*' filter='?uglifyjs2' %}
            <script src="{{ asset_url }}"></script>
        {% endjavascripts %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->javascripts(
            array('@AppBundle/Resources/public/js/*'),
            array('?uglifyjs2')
        ) as $url): ?>
            <script src="<?php echo $view->escape($url) ?>"></script>
        <?php endforeach ?>

Dla wypróbowania tego, przełącz swoje środowisko ``prod`` (``app.php``), ale
najpierw nie zapomnij :ref:`wyczyścić pamięć podręczną <book-page-creation-prod-cache-clear>`
i :ref:`zrzucić aktywa Assetic <cookbook-assetic-dump-prod>`.

.. tip::

    Zamiast dodawać filtry do znaczników aktywów, mozna również określić
    w pliku konfiguracyjnym filtry, jakie mają być zastosowane dla każdego pliku
    JavaScript.
    Zobacz :ref:`cookbook-assetic-apply-to` w celu uzyskania więcej informacji.

Instalacja, konfiguracja i stosowanie UglifyCSS
-----------------------------------------------

UglifyCSS działa bardzo podobnie do UglifyJS. W pierwszej kolejności, trzeba się
upewnić, czy zainstalowany jest pakiet *node*:

.. code-block:: bash

    # global installation
    $ npm install -g uglifycss

    # local installation
    $ cd /path/to/your/symfony/project
    $ npm install uglifycss --prefix app/Resources

Następnie trzeba dodać konfigurację dla tego filtra:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        assetic:
            filters:
                uglifycss:
                    bin: /usr/local/bin/uglifycss

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <assetic:config>
            <assetic:filter
                name="uglifycss"
                bin="/usr/local/bin/uglifycss" />
        </assetic:config>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('assetic', array(
            'filters' => array(
                'uglifycss' => array(
                    'bin' => '/usr/local/bin/uglifycss',
                ),
            ),
        ));

W celu użycia filtra dla plików CSS trzeba dodać filtr do helpera ``stylesheets``:

.. configuration-block::

    .. code-block:: html+jinja

        {% stylesheets 'bundles/App/css/*' filter='uglifycss' filter='cssrewrite' %}
             <link rel="stylesheet" href="{{ asset_url }}" />
        {% endstylesheets %}

    .. code-block:: html+php

        <?php foreach ($view['assetic']->stylesheets(
            array('bundles/App/css/*'),
            array('uglifycss'),
            array('cssrewrite')
        ) as $url): ?>
            <link rel="stylesheet" href="<?php echo $view->escape($url) ?>" />
        <?php endforeach ?>

Tak jak z filtrem ``uglifyjs2``, jeśli poprzedzi się nazwę filtra znakiem zapytania
``?`` (np. ``?uglifycss``), minifikacja bedzie wykonywana tylko, jeśli aplikacja
nie znajduje sie w trybie debugowania.

.. _`UglifyJS`: https://github.com/mishoo/UglifyJS
.. _`UglifyCSS`: https://github.com/fmarcia/UglifyCSS
.. _`Node.js`: http://nodejs.org/
.. _`zainstalować Node.js`: http://nodejs.org/
.. _`package.json`: http://package.json.nodejitsu.com/
