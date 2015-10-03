.. index::
   single: kontroler; dostosowywanie stron błędów
   single: strony błędów

Jak dostosować strony błędów
============================

W aplikacjach Symfony, wszystkie błędy są traktowane jako wyjątki, nie wazne czy
jest to błąd *404 Not Found* czy krytyczny błąd wyzwalany przez zrzucenie w kodzie
jakichś wyjątków.

W :doc:`środowisku programistycznym </cookbook/configuration/environments>`,
Symfony wychwytuje wszystkie wyjatki i wyświetla specjalną **strone wyjątku**
z wieloma informacjami debugowania pomocnymi w szybkim odkryciu głównego problemu:

.. image:: /images/cookbook/controller/error_pages/exceptions-in-dev-environment.png
   :alt: Typowa strona wyjątku wyświetlana w środowisku programistycznym

Ponieważ takie strony zawierają wiele poufnych informacji, Symfony nie wyświetla
ich w środowisku produkcyjnym. Zamiast tego wyswietlana jest prosta generyczna
**strona błędu**:

.. image:: /images/cookbook/controller/error_pages/errors-in-prod-environment.png
   :alt: Typowa strona bledu wyświetlana w środowisku produkcyjnym

Strony błedów dla środowiska produkcyjnego mogą zostać dostosowane na kilka sposobów,
w zalezności od potrzeb:

#. Jeśli chce się zmienić treść i stylizowanie stron błedów, aby dopasować je do
   reszty aplikacji, trzeba :ref:`przesłonić domyślne szablony stron błedów <use-default-exception-controller>`;

#. Jeśli chce się też dostosować logike stosowana przez Symfony do generowania
    stron błedów, trzba :ref:`przesłonić domyślny kontroler wyjątków <custom-exception-controller>`;

#. Jeśli jest potrzeba całkowitej kontroli nad obsługą wyjątków, aby wykonywać
   swoja wlasną logikę, trzeba :ref:`posłużyć się zdarzeniem kernel.exception <use-kernel-exception-event>`.


.. _use-default-exception-controller:
.. _using-the-default-exceptioncontroller:

Przesłanianie domyślnych szablonów stron błędów
-----------------------------------------------

Gdy ładowana jest strona błędów, do renderowania szablonu Twiga używana jest
wewnętrznie klasa :class:`Symfony\\Bundle\\TwigBundle\\Controller\\ExceptionController`,
dzięki czemu wyświetlana zostaje strona błędu.

.. _cookbook-error-pages-by-status-code:

Kontroler ten wykorzystuje kod stanu HTTP, format żądania i następującą logikę
ustalającą nazwę pliku szablonu:

#. Wyszukanie szablonu dla danego formatu i kodu stanu (takiego jak ``error404.json.twig``
   lub ``error500.html.twig``);

#. Jeśli poprzedni szablon nie istnieje, pomijany jest kod stanu i wyszukiwany
   jest generyczny szablon discard the status code and look for
   a generic template for the given format (like ``error.json.twig`` or
   ``error.xml.twig``);

#. Jeśli i ten szablon nie istnieje, wykorzystywany jest awaryjnie generyczny
   szablon HTML (``error.html.twig``).

.. _overriding-or-adding-templates:

Dla przesłoniecia tych szablonów wystarczy po prostu oprzeć się na standardowym
sposobie Symfony dla 
:ref:`przesłaniania szablonów umieszczonych w pakietach <overriding-bundle-templates>`:
umieszczając je w katalogu ``app/Resources/TwigBundle/views/Exception/``.

Typowy projekt, który zwraca strony HTML i JSON może wyglądać tak:

.. code-block:: text

    app/
    └─ Resources/
       └─ TwigBundle/
          └─ views/
             └─ Exception/
                ├─ error404.html.twig
                ├─ error403.html.twig
                ├─ error.html.twig      # Wszystkie inne błędy HTML (w tym 500)
                ├─ error404.json.twig
                ├─ error403.json.twig
                └─ error.json.twig      # Wszystkie inne błędy HTML (w tym 500)

Przykład szablonu błędu 404
---------------------------

W celu przesłonięcia szablonu błędu 404 dla strony HTML, utworzymy nowy szablon
``error404.html.twig`` umieszczony w ``app/Resources/TwigBundle/views/Exception/``:

.. code-block:: html+jinja

    {# app/Resources/TwigBundle/views/Exception/error404.html.twig #}
    {% extends 'base.html.twig' %}

    {% block body %}
        <h1>Page not found</h1>

        {# example security usage, see below #}
        {% if app.user and is_granted('IS_AUTHENTICATED_FULLY') %}
            {# ... #}
        {% endif %}

        <p>
            The requested page couldn't be located. Checkout for any URL
            misspelling or <a href="{{ path('homepage') }}">return to the homepage</a>.
        </p>
    {% endblock %}

W przypadku gdy zachodzi taka potrzeba, ``ExceptionController`` przekazuje kilka
informacji do szablonu błędu poprzez zmienne ``status_code`` i ``status_text``,
które przechowuja kod stanu HTTP i odpowiedni komunikat.

.. tip::

    Można dostosować kod stanu implementując intefejs
    :class:`Symfony\\Component\\HttpKernel\\Exception\\HttpExceptionInterface`
    i jego obowiązkową metodę ``getStatusCode()``. W przeciwnym razie ``status_code``
    będzie miał domyślną wartość ``500``.

.. note::

    Strony wyjątków wyświetlane w środowisku programistycznym można dostosować
    w ten sam sposób, co strony błędów. Trzeba utworzyć nowy szablon
    ``exception.html.twig`` dla standardowej strony HTML wyjątku,
    a ``exception.json.twig`` dla strony JSON wyjątku.

Problem z zrzucaniem wyjątków, gdy używa się funkcji bezpieczeństwa w szablonach błędów
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Jedną z najczęstszych pułapek przy projektowaniu własnych stron błedów jest użycie
funkcji ``is_granted()`` w szablonie błedów (lub jakimkolwiek szablonie dziedziczonym
przez szablon błędów). Gdy sie to zrobi, to doświadczy się zrzucenia wyjątku przez
Symfony.

Przyczyną tego problemu jest to, że trasowanie jest wykonywane przed bezpieczeństwem.
Jeśli wystąpi błąd 404, warstwa bezpieczeństwa nie będzie załadowana i tym samym
funkcja ``is_granted()`` będzie nie zdefiniowana. Rozwiązaniem jest dodanie
nastęþującego kodu sprawdzającego przed użyciem tej funkcji:

.. code-block:: jinja

    {% if app.user and is_granted('...') %}
        {# ... #}
    {% endif %}

.. _testing-error-pages:

Testowanie stron błędów podczas prac programistycznych
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Gdy sie pracuje w środowisku programistycznym, Symfony wyswietla dużą stronę
*exception* zamiast co dopiero przygotowanej przez programistę własnej strony
błędu. Tak więc jak można ją debugować?

Zalecanym rozwiązaniem (do wersji 2.6) było zastosowanie pakietu zewnętrznego o nazwie
`WebfactoryExceptionsBundle`_.
Pakiet ten dostarcza specjalny kontroler testowy, który umożliwia łatwe wyświetlanie
własnych stron błędów dla kodów stanu HTTP, nawet jeśłi ``kernel.debug`` jest
ustawiony na ``true``.

.. versionadded:: 2.6
    Od wersji 2.6 Symfony, standardowy kontroler ``ExceptionController`` również
    umożliwia podgląd swoich stron błędów w środowisku programistycznym.
    Tak więc pakiet `WebfactoryExceptionsBundle`_ nie musi być juz stosowany
    w tym celu.

Obecnie, aby użyć tą funkcjonalność, trzeba zdefiniować w pliku ``routing_dev.yml``
cos takiego:

.. configuration-block::

    .. code-block:: yaml

        # app/config/routing_dev.yml
        _errors:
            resource: "@TwigBundle/Resources/config/routing/errors.xml"
            prefix:   /_error

    .. code-block:: xml

        <!-- app/config/routing_dev.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <import resource="@TwigBundle/Resources/config/routing/errors.xml"
                prefix="/_error" />
        </routes>

    .. code-block:: php

        // app/config/routing_dev.php
        use Symfony\Component\Routing\RouteCollection;

        $collection = new RouteCollection();
        $collection->addCollection(
            $loader->import('@TwigBundle/Resources/config/routing/errors.xml')
        );
        $collection->addPrefix("/_error");

        return $collection;

Jeśli pracuje się w aplikacji utworzonej w starszej wersji Symfony, może być
konieczne dodanie powyższego kodu do pliku ``routing_dev.yml``, bo w aplikacjach
wygenerowanych w `Symfony Standard Edition`_ (wersja >= 2.6) kod ten jest już
dodany do tego pliku.

Po dodaniu tej trasy można uzyć adresów URL, takich jak:

.. code-block:: text

     http://localhost/app_dev.php/_error/{statusCode}
     http://localhost/app_dev.php/_error/{statusCode}.{format}

do przeglądania stron błędów dla określonego kodu stanu w wybranym formacie
(nie tylko HTML).

.. _custom-exception-controller:
.. _replacing-the-default-exceptioncontroller:

Przesłanianie standardowego ExceptionController
-----------------------------------------------

Jeśłi konieczna jest większa elastyczność, niż tylko przesłonięcie szablonu,
można tzmienić kontroler, który renderuje strone błędu.

Przyjmijmy na przykład, że potrzebujemy przekazać do szablonu kilka dodatkowych
zmiennych. W celu wykonania tego, trzeba utworzyć gdziekolwiek w aplikacji
nowy kontroler i ustawić
opcję konfiguracyjna :ref:`twig.exception_controller <config-twig-exception-controller>`
tak, aby wskazywała nowy kontroler:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        twig:
            exception_controller:  AppBundle:Exception:showException

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:twig="http://symfony.com/schema/dic/twig"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/twig
                http://symfony.com/schema/dic/twig/twig-1.0.xsd">

            <twig:config>
                <twig:exception-controller>AppBundle:Exception:showException</twig:exception-controller>
            </twig:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('twig', array(
            'exception_controller' => 'AppBundle:Exception:showException',
            // ...
        ));

Klasa :class:`Symfony\\Component\\HttpKernel\\EventListener\\ExceptionListener`
używana przez TwigBundle jako detektor zdarzenia ``kernel.exception`` tworzy
żądanie, które zostaje wysłane do naszego kontrolera. Ponadto kontroler bedzie
przekazywał dwa parametry:

``exception``
    Instancja klasy :class:`\\Symfony\\Component\\Debug\\Exception\\FlattenException`
    utworzona z obsługiwanego wyjątku.

``logger``
    Instancja klasy :class:`\\Symfony\\Component\\HttpKernel\\Log\\DebugLoggerInterface`,
    która może być ``null`` w pewnych okolicznościach.

Zamiast od podstaw tworzyć nowy kontroler wyjątku, można oczywiście rozszerzyć
standardową klasę :class:`Symfony\\Bundle\\TwigBundle\\Controller\\ExceptionController`.
W takim przypadku, bedzie można przesłonić jedną lub obie metody``showAction()``
i ``findTemplate()``. Ta druga metoda ustala szablon do użycia.

.. tip::

    :ref:`Podgląd strony błędów <testing-error-pages>` działa również dla własnego
    kontrolera skonfigurowanego w ten sposób.

.. _use-kernel-exception-event:

Praca ze zdarzeniem ``kernel.exception``
----------------------------------------

Gdy zrzucany jest wyjątek, klasa :class:`Symfony\\Component\\HttpKernel\\HttpKernel`
wyłapuje go i wyzwala zdarzenie ``kernel.exception``. Daje to możliwość
przekształcenia wyjątku w obiekt ``Response``, na kilka różnych sposobów.

Praca z tym zdarzeniem dje o wiele większe możliwości niż to co dotychczas
omówiliśmy, ale jednocześnie wymaga dogłębnego zrozumienia wewnętrznych mechanizmów
Symfony. Załóżmy, że nasz kod zrzuca specjalistyczny wyjątek o szczególnym znaczeniu
dla domeny aplikacji.

:doc:`Pisząc własny detektor </cookbook/event_dispatcher/event_listener>`
dla zdarzenia ``kernel.exception`` umożliwia się bliższe zapoznanie sie z wyjatkiem
i podjęcie róznych działań. Dzialania te moga obejmować rejestrowanie wyjatku,
przekierowanie do innej strony lub wyrenderowanie specjalistycznych stron błędów.

.. note::

    Jeśli detektor wywołuje ``setResponse()`` na klasie
    :class:`Symfony\\Component\\HttpKernel\\Event\\GetResponseForExceptionEvent`,
    propagacja zdarzenia zostanie zatrzymana a odpowiedź będzie przesłana do klienta.

Takie podejście umożliwia stworzeniescentralizowanej i warstwowej obsługi błedów.
Zamiast wyłapywać (i obsługiwać) raz po raz te same błędy w różnych kontrolerach,
wystarczy mieć tylko jeden lub kilka detektorów, z którymi łatwiej sobie radzić.

.. tip::

    Przykład rzeczywistego detektora tego typu można znaleźć w kodzie klasy
    :class:`Symfony\\Component\\Security\\Http\\Firewall\\ExceptionListener`.
    Detektor ten obsługuje rózne wyjątki zwiazane z bezpieczeństwem, które są
    zrzucane w aplikacji
    (jak :class:`Symfony\\Component\\Security\\Core\\Exception\\AccessDeniedException`)
    i przyjmują działania takie jak przekierowywanie uzytkownika do strony logowania,
    wylogowywanie użytkownika i inne rzeczy.

.. _`WebfactoryExceptionsBundle`: https://github.com/webfactory/exceptions-bundle
.. _`Symfony Standard Edition`: https://github.com/symfony/symfony-standard/
.. _`ExceptionListener`: https://github.com/symfony/symfony/blob/master/src/Symfony/Component/Security/Http/Firewall/ExceptionListener.php
.. _`development environment`: http://symfony.com/doc/current/cookbook/configuration/environments.html
