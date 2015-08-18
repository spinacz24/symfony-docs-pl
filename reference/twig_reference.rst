.. index::
    single: Twig; rozszerzenie dla Symfony

.. _symfony2-twig-extensions:

Rozszerzenie Twig dla Symfony
=============================

W Symfony domyślnym silnikiem szablonowania jest Twig. Sam w sobie, zawiera on
wiele wbudowanych funkcji, filtrów, znaczników i testów (w celu ich poznania,
zapoznaj sie z `Informatorem Twig`_).

Symfony dodaje do niego wiele własnym rozszerzeń w celu integracji swoich
komponentów z szablonami Twiga. W dalszej części tego rozdziału opisane są własne
:ref:`funkcje <reference-twig-functions>`, :ref:`filtry <reference-twig-filters>`,
:ref:`znaczniki <reference-twig-tags>` i :ref:`testy <reference-twig-tests>`
dostępne w czasie używania rdzenia frameworka Symfony.

W niektórych pakietach mogą znajdować się też znaczniki, tutaj nie ujęte.

.. index::
    single: Twig; funkcje

.. _reference-twig-functions:

Funkcje
-------

.. _reference-twig-function-render:

render
~~~~~~

.. code-block:: jinja

    {{ render(uri, options) }}

``uri``
    **typ**: ``string`` | ``ControllerReference``
``options``
    **typ**: ``array`` **domyślnie**: ``[]``

Renderuje fragment zawartości dla określonego kontrolera (używając funkcję `controller`_)
lub URI. Więcej informcji: see :ref:`templating-embedding-controller`.

Strategia renderowania może zostać określona w kluczu ``strategy`` parametru ``options``.

.. tip::

    Adres URI może być generowany przez inne funkcje, takie jak `path`_ i `url`_.

.. _reference-twig-function-render-esi:

render_esi
~~~~~~~~~~

.. code-block:: jinja

    {{ render_esi(uri, options) }}

``uri``
    **typ**: ``string`` | ``ControllerReference``
``options``
    **typ**: ``array`` **domyślnie**: ``[]``

Generuje znacznik ESI, gdy to możliwe, w przciwnym razie powraca do zachowania
funkcji `render`_. Więcej informcji: :ref:`templating-embedding-controller`.

.. tip::

    Adres URI może być generowany przez inne funkcje, takie jak `path`_ i `url`_.

.. tip::

    Funkcja ``render_esi()`` jest przykładem jednego z wielu skrótów funkcji ``render``.
    Automatycznie ustawia strategię w oparciu o to, co podano w nazwie funkcji,
    np. ``render_hinclude()`` użyje strategii hinclude.js. To działa na wszystkich
    funkcjach ``render_*()``.

controller
~~~~~~~~~~

.. code-block:: jinja

    {{ controller(controller, attributes, query) }}

``controller``
    **typ**: ``string``
``attributes``
    **typ**: ``array`` **domyślnie**: ``[]``
``query``
    **typ**: ``array`` **domyślnie**: ``[]``

Zwraca instancję ``ControllerReference``, aby uzyć ją w funkcjach takich jak
:ref:`render() <reference-twig-function-render>` i
:ref:`render_esi() <reference-twig-function-render-esi>`.

asset
~~~~~

.. code-block:: jinja

    {{ asset(path, packageName, absolute = false, version = null) }}

``path``
    **typ**: ``string``
``packageName``
    **typ**: ``string`` | ``null`` **domyślnie**: ``null``
``absolute`` (przestarzałe od 2.7)
    **typ**: ``boolean`` **domyślnie**: ``false``
``version`` (przestarzałe od 2.7)
    **typ**: ``string`` **domyślnie** ``null``

Zwraca publiczną ścieżkę do ``path``, która uwzględnia ścieżkę bazową ustawioną
dla zestawu aktywów i ścieżkę URL. Więcej informacji na ten temat można znaleźć
w rozdziale :ref:`book-templating-assets`. W celu zapoznania się z wersjonowaniem
aktywów, proszę zapoznac się z :ref:`ref-framework-assets-version`.

assets_version
~~~~~~~~~~~~~~

.. code-block:: jinja

    {{ assets_version(packageName) }}

``packageName``
    **typ**: ``string`` | ``null`` **domyślnie**: ``null``

Zwraca bieżącą wersję zestawu aktywów. Więcej informacji: :ref:`book-templating-assets`.

form
~~~~

.. code-block:: jinja

    {{ form(view, variables) }}

``view``
    **typ**: ``FormView``
``variables``
    **typ**: ``array`` **domyślnie**: ``[]``

Renderuje kod HTML kompletnego formularza. Więcej informacji:
:ref:`Informator formularzy Twig <reference-forms-twig-form>`.

form_start
~~~~~~~~~~

.. code-block:: jinja

    {{ form_start(view, variables) }}

``view``
    **typ**: ``FormView``
``variables``
    **typ**: ``array`` **domyślnie**: ``[]``

Renderuje znacznik poczatkowy HTML formularza. Więcej informacji:
:ref:`Informator formularzy Twig <reference-forms-twig-start>`.

form_end
~~~~~~~~

.. code-block:: jinja

    {{ form_end(view, variables) }}

``view``
    **typ**: ``FormView``
``variables``
    **typ**: ``array`` **domyślnie**: ``[]``

Renderuje znacznik końcowy HTML formularza ze wszystkimi polami, które nie mają
być jeszcze zrenderowane. Więcej informacji:
:ref:`Informator formularzy Twig <reference-forms-twig-end>`.

form_enctype
~~~~~~~~~~~~

.. code-block:: jinja

    {{ form_enctype(view) }}

``view``
    **typ**: ``FormView``

Renderuje atrybut wymagania ``enctype="multipart/form-data"``, jeśli formularz
zawiera co najmniej jedno pole przesyłania pliku. Więcej informacji:
:ref:`Informator formularzy Twig <reference-forms-twig-enctype>`.

form_widget
~~~~~~~~~~~

.. code-block:: jinja

    {{ form_widget(view, variables) }}

``view``
    **typ**: ``FormView``
``variables``
    **typ**: ``array`` **domyślnie**: ``[]``

Renderuje cały formularza lub określony widżet HTML pola. Więcej informacji:
:ref:`Informator formularzy Twig <reference-forms-twig-widget>`.

form_errors
~~~~~~~~~~~

.. code-block:: jinja

    {{ form_errors(view) }}

``view``
    **typ**: ``FormView``

Renderuje wszystkie błędy dla danego pola lub błędy ogólne. Więcej informacji:
:ref:`Informator formularzy Twig <reference-forms-twig-errors>`.

form_label
~~~~~~~~~~

.. code-block:: jinja

    {{ form_label(view, label, variables) }}

``view``
    **typ**: ``FormView``
``label``
    **typ**: ``string`` **domyślnie**: ``null``
``variables``
    **typ**: ``array`` **domyślnie**: ``[]``

Renderuje etykietę określonego pola. Więcej informacji:
:ref:`Informator formularzy Twig <reference-forms-twig-label>`.

form_row
~~~~~~~~

.. code-block:: jinja

    {{ form_row(view, variables) }}

``view``
    **typ**: ``FormView``
``variables``
    **typ**: ``array`` **domyślnie**: ``[]``

Renderuje wiersz (etykietę pola, błędy i widżet) określonego pola.
Więcej informacji: :ref:`Informator formularzy Twig <reference-forms-twig-row>`.

form_rest
~~~~~~~~~

.. code-block:: jinja

    {{ form_rest(view, variables) }}

``view``
    **typ**: ``FormView``
``variables``
    **typ**: ``array`` **domyślnie**: ``[]``

Renderuje wszystkie pola, które nie maja być jeszcze zrenderowane. Więcej informacji:
:ref:`Informator formularzy Twig <reference-forms-twig-rest>`.

csrf_token
~~~~~~~~~~

.. code-block:: jinja

    {{ csrf_token(intention) }}

``intention``
    **typ**: ``string``

Renderuje token CSRF. Użyj tą funkcję, jeśli chcesz uzyskać ochronę CSRF bez
tworzenia formularza.

is_granted
~~~~~~~~~~

.. code-block:: jinja

    {{ is_granted(role, object, field) }}

``role``
    **typ**: ``string``
``object``
    **typ**: ``object``
``field``
    **typ**: ``string``

Zwraca ``true``, jeśli bieżący użytkownik posiada wymaganą rolę. Opcjonalnie,
można wkleić obiekt, aby był wykorzystany przez wyborcę. Więcej informacji można
znaleźć w :ref:`book-security-template`.

.. note::

    Można również przekazać w polu użycie ACE dla tego pola. Proszę przeczytać
    na ten temat w :ref:`cookbook-security-acl-field_scope`.

logout_path
~~~~~~~~~~~

.. code-block:: jinja

    {{ logout_path(key) }}

``key``
    **typ**: ``string``

Generuje wzgledną ścieżkę URL wylogowania dla określonej zapory.

logout_url
~~~~~~~~~~

.. code-block:: jinja

    {{ logout_url(key) }}

``key``
    **typ**: ``string``

Równoważne z funkcją `logout_path`_, ale generuje bezwzględną ścieżkę URL
zamiast ścieżki względnej.

path
~~~~

.. code-block:: jinja

    {{ path(name, parameters, relative) }}

``name``
    **typ**: ``string``
``parameters``
    **typ**: ``array`` **domyślnie**: ``[]``
``relative``
    **typ**: ``boolean`` **domyślnie**: ``false``

Zwraca względną ścieżkę URL (bez schematu i hosta) dla danej trasy.
Jeśli włączony jest ``relative``, to będzie tworzona ścieżka względem bieżącej
ścieżki. Więcej informacji: :ref:`book-templating-pages`.

url
~~~

.. code-block:: jinja

    {{ url(name, parameters, schemeRelative) }}

``name``
    **typ**: ``string``
``parameters``
    **typ**: ``array`` **domyślnie**: ``[]``
``schemeRelative``
    **typ**: ``boolean`` **domyślnie**: ``false``

Zwraca bewzględny adres URL (ze schematem i hostem) dla danej trasy. Jeśłi włączony
jest ``schemeRelative``, to tworzony będzie adres URL o schemacie wzglednym. Więcej
informacji: :ref:`book-templating-pages`.

absolute_url
~~~~~~~~~~~~

.. versionadded:: 2.6
     The ``absolute_url`` function was introduced in Symfony 2.7

.. code-block:: jinja

    {{ absolute_url(path) }}

``path``
    **typ**: ``string``

Zwraca bezwzględny adres URL dla danej bezwzglednej ścieżki. Jest to przydatne
do konwersji istniejącej ścieżki:

.. code-block:: jinja

    {{ absolute_url(asset(path)) }}

relative_path
~~~~~~~~~~~~~

.. versionadded:: 2.6
     Funkcję ``relative_path`` wprowadzono w Symfony 2.7

.. code-block:: jinja

    {{ relative_path(path) }}

``path``
    **typ**: ``string``

Zwraca względną ścieżkę dla podanej ściezki bezwzglednej (w oparciu o bieżącą
ścieżkę żądania). Na przykład, jeśli bieżącą ścieżką jest ``/article/news/welcome.html``,
to względną ścieżką dla ``/article/image.png`` jest ``../images.png``.

expression
~~~~~~~~~~

Tworzy w Twig klasę :class:`Symfony\\Component\\ExpressionLanguage\\Expression`.
Zobacz ":ref:`Wyrażenia szablonowe <book-security-template-expression>`".



.. index::
    single: Twig; filtry

.. _reference-twig-filters:

Filtry
------

humanize
~~~~~~~~

.. code-block:: jinja

    {{ text|humanize }}

``text``
    **typ**: ``string``

Przekształca techniczną nazwę w czytelny tekst (czyli zamienia znaki podkreślenia
przez spacje i kapitalizuje łańcuch).

trans
~~~~~

.. code-block:: jinja

    {{ message|trans(arguments, domain, locale) }}

``message``
    **typ**: ``string``
``arguments``
    **typ**: ``array`` **domyślnie**: ``[]``
``domain``
    **typ**: ``string`` **domyślnie**: ``null``
``locale``
    **typ**: ``string`` **domyślnie**: ``null``

Tłumaczy tekst na bieżący język. Więcej informacji:
:ref:`Filtry tłumaczeń <book-translation-filters>`.

transchoice
~~~~~~~~~~~

.. code-block:: jinja

    {{ message|transchoice(count, arguments, domain, locale) }}

``message``
    **typ**: ``string``
``count``
    **typ**: ``integer``
``arguments``
    **typ**: ``array`` **domyślnie**: ``[]``
``domain``
    **typ**: ``string`` **domyślnie**: ``null``
``locale``
    **typ**: ``string`` **domyślnie**: ``null``

Tłumaczy tekst z obsługą liczby mnogiej. Więcej informacji:
:ref:`Filtry tłumaczeń <book-translation-filters>`.

yaml_encode
~~~~~~~~~~~

.. code-block:: jinja

    {{ input|yaml_encode(inline, dumpObjects) }}

``input``
    **typ**: ``mixed``
``inline``
    **typ**: ``integer`` **domyślnie**: ``0``
``dumpObjects``
    **typ**: ``boolean`` **domyślnie**: ``false``

Przeksztalca dane wejściowe na składnię YAML. Zobacz :ref:`components-yaml-dump`
w celu uzyskania wiecej informacji.

yaml_dump
~~~~~~~~~

.. code-block:: jinja

    {{ value|yaml_dump(inline, dumpObjects) }}

``value``
    **typ**: ``mixed``
``inline``
    **typ**: ``integer`` **domyślnie**: ``0``
``dumpObjects``
    **typ**: ``boolean`` **domyślnie**: ``false``

Robi to samo co `yaml_encode() <yaml_encode>`_, ale zawiera na wyjściu wydruk.

abbr_class
~~~~~~~~~~

.. code-block:: jinja

    {{ class|abbr_class }}

``class``
    **typ**: ``string``

Generuje element ``<abbr>`` z krótka nazwą klasy PHP
(w pełni kwalifikowana nazwa klasy (FQCN) będzie sie pokazywać w dymku, gdy użytkownik
najedzie kursorem myszki na ten element).

abbr_method
~~~~~~~~~~~

.. code-block:: jinja

    {{ method|abbr_method }}

``method``
    **typ**: ``string``

Generuje element ``<abbr>`` używając składnię ``FQCN::method()``. Jeśli
``method`` to ``Closure``, zamiast tego użyte zostanie ``Closure`` a jeśli ``method``
nie ma nazwy klasy, zostanie wyświetlona jako funkcja (``method()``).

format_args
~~~~~~~~~~~

.. code-block:: jinja

    {{ args|format_args }}

``args``
    **typ**: ``array``

Generuje łańcuch tekstowy z argumentami i ich typami (w elementach ``<em>``).

format_args_as_text
~~~~~~~~~~~~~~~~~~~

.. code-block:: jinja

    {{ args|format_args_as_text }}

``args``
    **typ**: ``array``

Równoważnik dla filtra `format_args`_, ale bez uzywania znaczników HTML.

file_excerpt
~~~~~~~~~~~~

.. code-block:: jinja

    {{ file|file_excerpt(line) }}

``file``
    **typ**: ``string``
``line``
    **typ**: ``integer``

Generuje urywek tekstu złożony z siedmiu linii wokół podanej linii ``line``.

format_file
~~~~~~~~~~~

.. code-block:: jinja

    {{ file|format_file(line, text) }}

``file``
    **typ**: ``string``
``line``
    **typ**: ``integer``
``text``
    **typ**: ``string`` **domyślnie**: ``null``

Generuje ścieżkę do pliku wewnątrz elementu ``<a>``. Jeśli ścieżka prowadzi do
głównego katalogu kernela, jest ona zamieniana na ``kernel.root_dir``
(pokazując pełną ścieżkę w dymku po najechaniu kursorem myszy na element).

format_file_from_text
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: jinja

    {{ text|format_file_from_text }}

``text``
    **typ**: ``string``

Używa `format_file`_, aby poprawić wyjście domyślnych błędów PHP.

file_link
~~~~~~~~~

.. code-block:: jinja

    {{ file|file_link(line) }}

``line``
    **typ**: ``integer``

Generuje odnośnik do podanego pliku (i ewentualnie numer linii) używając
prekonfigurowany schemat.

.. index::
    single: Twig; znaczniki

.. _reference-twig-tags:

Znaczniki
---------

form_theme
~~~~~~~~~~

.. code-block:: jinja

    {% form_theme form resources %}

``form``
    **typ**: ``FormView``
``resources``
    **typ**: ``array`` | ``string``

Ustawia zasoby do przesłonięcia motywu formularza dla danej instancji widoku
formularza.
Jako zasoby można użyć ``_self``, aby ustawić znacznik na bieżący zasób. Więcej
informacji można znaleźć w artykule :doc:`/cookbook/form/form_customization`.

trans
~~~~~

.. code-block:: jinja

    {% trans with vars from domain into locale %}{% endtrans %}

``vars``
    **typ**: ``array`` **domyślnie**: ``[]``
``domain``
    **typ**: ``string`` **domyślnie**: ``string``
``locale``
    **typ**: ``string`` **domyślnie**: ``string``

Renderuje tłumaczenie treści. Więcej informacji: :ref:`book-translation-tags`.

transchoice
~~~~~~~~~~~

.. code-block:: jinja

    {% transchoice count with vars from domain into locale %}{% endtranschoice %}

``count``
    **typ**: ``integer``
``vars``
    **typ**: ``array`` **domyślnie**: ``[]``
``domain``
    **typ**: ``string`` **domyślnie**: ``null``
``locale``
    **typ**: ``string`` **domyślnie**: ``null``

Renderuje tłumaczenie treści z obsługą liczby mnogiej. Wiecej informacji:
:ref:`book-translation-tags`.

trans_default_domain
~~~~~~~~~~~~~~~~~~~~

.. code-block:: jinja

    {% trans_default_domain domain %}

``domain``
    **typ**: ``string``

Ustawia domyślną domenę w bieżącym szablonie.

stopwatch
~~~~~~~~~

.. code-block:: jinja

    {% stopwatch 'name' %}...{% endstopwatch %}

Mierzy czas wykonania kodu zawartego w znaczniku i wstawia tą wartość na linii
czasu WebProfilerBundle.


.. index::
    single: Twig; testy

.. _reference-twig-tests:

Testy
-----

selectedchoice
~~~~~~~~~~~~~~

.. code-block:: jinja

    {% if choice is selectedchoice(selectedValue) %}

``choice``
    **typ**: ``ChoiceView``
``selectedValue``
    **typ**: ``string``

Sprawdza czy ``selectedValue`` będzie zaznaczone w podanym polu wyboru.
Używanie tego testu jest bardzo skuteczne.


.. index::
    single: Twig; zmienne globalne

.. _reference-twig-global-app:

Zmienne globalne
----------------

app
~~~

Zmienna ``app`` jest dostępna w całym szablonie i daje dostęp do wielu powszechnie
potrzebnych obiektów i wartości. jest to instancja
:class:`Symfony\\Bundle\\FrameworkBundle\\Templating\\GlobalVariables`.

Dostępne atrybuty, to:

* ``app.user``
* ``app.request``
* ``app.session``
* ``app.environment``
* ``app.debug``
* ``app.security``

.. versionadded:: 2.6
     Zmienna globalna ``app.security`` jest przestarzała od wersji 2.6.
     Obiekt user jest teraz dostępny jako ``app.user`` a ``is_granted()``
     zostało zarejestrowne jako funkcja.

.. index::
    single: Twig; rozszerzenia dla Symfony SE

Rozszerzenia Symfony Standard Edition
-------------------------------------

Symfony Standard Edition dodaje kilka pakietów do Symfony2 Core Framework.
Pakiety te mogą mieć inne rozszerzenia Twig:

* **Rozszerzenie Twig** dołącza wszystkie rozszerzenia, które nie należą do rdzenia
  Twig, ale mogą być interesujace. Czytaj więcej na ten temat w
  `oficjalnej dokumentacji rozszerzeń Twig`_
* **Assetic** dodaje znaczniki ``{% stylesheets %}``, ``{% javascripts %}`` i 
  ``{% image %}``. Można przeczytać więcej na ten temat w 
  :doc:`dokumentacji Assetic </cookbook/assetic/asset_management>`;


.. _`Informatorem Twig`: http://twig.sensiolabs.org/documentation#reference 
.. _`oficjalnej dokumentacji rozszerzeń Twig`: http://twig.sensiolabs.org/doc/extensions/index.html