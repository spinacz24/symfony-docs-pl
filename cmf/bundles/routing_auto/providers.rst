.. index::
    single: dostawcy ścieżek; RoutingAutoBundle
    
Dostawcy ścieżek
----------------

Dostawcy ścieżek określają docelową ścieżkę wykorzystywaną przez kolejne akcje
ścieżek do dostarczania rzeczywistych dokumentów trasy.

W łańcuchu ścieżek treści muszą najpierw zostać skonfigurowani **bazowi** dostawcy.
Powodem tego jest to, że ścieżki dostarczane przez nich odpowiadają bezpośrednio
istniejącej ścieżce, czyli mają bezwzględne odniesienie.

specified (bazowy dostawca)
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Jest to najbardziej podstawowy dostawca ścieżek i zawsze pozwala określić dokładną
(stałą) ścieżkę.

Opcje
.....

* ``path`` - **wymagana** Zapewniona ścieżka.

.. configuration-block::

    .. code-block:: yaml

        provider: [specified, { path: this/is/a/path }]

    .. code-block:: xml

        <provider name="specified">
            <option name="path" value="this/is/a/path" />
        </provider>

    .. code-block:: php

        array(
            // ...
            'provider' => array('specified', array('path' => 'this/is/a/path')),
        );

.. caution::

    W systemie automatycznych tras nigdy nie należy określać bezwzględnych ścieżek.
    Jeśli konstruktor jednostek jest pierwszą treścią łańcucha ścieżek, to jest
    zrozumiałe, że jest to bazowa ścieżki bezwzględna.

content_object (dostawca bazowy)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Dostawca obiektu treści próbuje dostarczać ścieżkę z obiektu implementującego
``RouteReferrersInterface`` pochodzącego z metody wskazanej w dokumentu treści.
Na przykład, jeśli ma się klasę ``Topic``, w której znajduje się metoda ``getCategory``,
to używając tego dostawcę można powiadomić automatyczną trasę ``Topic``, aby
wykorzystywała trasę kategorii jako bazową.

Tak więc w zasadzie, jeśli dokument kategorii ma ścieżkę ``/to/jest/moja/kategoria``,
to można wykorzystać tą ściezkę jako bazę automatycznych tras ``Category``.

Opcje
.....

 - ``method``: **wymagana** Metoda stosowana do zwracania dokumentu, ze ścieżki
   którego chce się skorzystać.

.. configuration-block::

    .. code-block:: yaml

        provider: [content_object, { method: getCategory }]

    .. code-block:: xml

        <provider name="content_object">
            <option name="method" value="getCategory" />
        </provider>

    .. code-block:: php

        array(
            // ...
            'provider' => array('content_object', array('method' => 'getCategory')),
        );

.. note::

    W czasie pisania tego artykułu tłumaczone obiekty nie były obsługiwane,
    ale poprawka dla tej funkcjonalności jest już stworzona.

content_method
~~~~~~~~~~~~~~

Dostawca ``content_method`` pozwala, aby obiekt treści (np. ``Topic`` forum)
określał ścieżkę wykorzystując jedna ze swoich metod. Jest to dość mocny sposób
pozwalający, aby dokument treści zrobił wszystko co możliwe, aby wytworzyć trasę.
Wadą jest to, że dokument będzie zawierał dodatkowy kod.

Opcje
.....

* ``method``: **wymagana** Metoda używana do zwracania elementów name/path/path trasy.
* ``slugify``: Czy zwracana wartość powinna zostać przekształcona w alias
   (konwertowana na krótka nazwę) , domyślnie ``true``.

.. configuration-block::

    .. code-block:: yaml

        provider: [content_method, { method: getTitle }]

    .. code-block:: xml

        <provider name="content_method">
            <option name="method" value="getTitle" />
        </provider>

    .. code-block:: php

        array(
            // ...
            'provider' => array('content_method', array('method' => 'getTitle')),
        );

W tym przykładzie będziemy używać istniejącej metody "getTitle" dokumentu ``Topic``,
aby pobrać tytuł. Domyślnie wszystkie ciągi tekstowe będą *aliasowane*.

Metoda ta może zwracać ścieżkę jako pojedynczy ciąg, tablica elementów ścieżki
albo obiekt, który może zostać przekształcony do ciągu, tak jak pokazano w następnym
przykładzie::

    class Topic
    {
        /* Using a string */
        public function getTitle()
        {
            return "This is a topic";
        }

        /* Using an array */
        public function getPathElements()
        {
            return array('this', 'is', 'a', 'path');
        }

        /* Using an object */
        public function getStringObject()
        {
            $object = ...; // an object which has a __toString() method

            return $object;
        }
    }

content_datetime
~~~~~~~~~~~~~~~~

Dostawca ``content_datettime`` będzie dostarczał ścieżkę z obiektu ``DateTime``
otrzymywanego przez wskazanie metody  w dokumencie treści.

.. configuration-block::

    .. code-block:: yaml

        provider: [content_datetime, { method: getDate, date_format: Y/m/d }]

    .. code-block:: xml

        <provider name="content_datetime">
            <option name="method" value="getDate" />
            <option name="date_format" value="Y/m/d" />
        </provider>

    .. code-block:: php

        array(
            // ...
            'provider' => array('content_datetime', array(
                'method' => 'getDate',
                'date_format' => 'Y/m/d',
            )),
        );

.. note::

    Metoda ta rozszerza `content_method`_ i dziedziczy cechę aliasowania.
    Wewnętrznie zwraca łańcuch znakowy używając metodę `DateTime->format()`.
    Oznacza to, że można określić datę w sposób jaki się chce i zostanie ona
    automatycznie aliasowana. Ponadto przez dodanie separatorów ścieżki w
    ``date_format`` można efektywnie tworzyć trasy dla każdego elementu daty,
    ponieważ aliasowanie dotyczy **każdego elementu** ścieżki.

Opcje
.....

* ``method``: **required** Metoda używana do zwracania elementów trasy name/path/path.
* ``slugify``: Czy zwracana wartość ma być aliasowana (przekształcona w krótką
  nazwę – slug), domyślnie ``true``.
* ``date_format``: Domyśłny format daty akceptowany przez klasę `DateTime`,
  domyślnie ``Y-m-d``.
