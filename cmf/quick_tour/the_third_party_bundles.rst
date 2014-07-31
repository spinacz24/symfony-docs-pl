.. index::
    single: społeczność; krótki kurs

Pakiety osób trzecich
=====================

Rozdział ten omawia krótko kilka ważnych pakietów wykorzystywanych w CMF.
Większość innych pakietów jest opartych na takich gigantachj jak KnpMenuBundle_
czy SonataAdminBundle_.

MenuBundle
----------

Rozpocznijmy od omówienia MenuBundle. Jeśli odwiedzi się stronę, można zobaczyć
ładne menu. Renderowanie tego menu znajduje się w układzie AcmeDemoBundle:

.. code-block:: html+jinja

    <!-- src/Acme/DemoBundle/Resources/views/layout.html.twig -->

    <!-- ... -->
    <nav class="navbar  navbar-inverse  page__nav" role="navigation">
        <div class="container-fluid">
            {{ knp_menu_render('simple', {'template': 'AcmeDemoBundle:Menu:bootstrap.html.twig', 'currentClass': 'active'}) }}

            <!-- ... -->
        </div>
    </nav>

Ja widać, menu jest renderowane przez znacznik ``knp_menu_render``. Może się to
wydawać trochę dziwne, że mówimy o CmfMenuBundle a nie o KnpMenuBundle ale w
rzeczywistości CmfMenuBundle jest cienka warstwą na szczycie KnpMenuBundle.

Zwykle argumentem ``knp_menu_render()`` jest nazwa renderowanego menu,
ale gdy używa się CmfMenuBundle, jest nim identyfikator węzła. W tym przypadku
menu zawiera wszystkie implementowane ``NodeInterface`` znajdujące się wewnątrz
``/cms/simple`` (ponieważ ścieżką bazową w Standard Edition jest ``/cms``).

.. note::

   Oprócz dołączenia  dostawca menu PHPCR, CmfMenuBundle również dostarcza klas
   interfejsu administracyjnego. W celu poznania szczegółów zobacz rozdział dotyczacy
   `Sonata Admin`_.

CreateBundle
------------

O pakiecie tym była już mowa w pierwszym rozdziale. Integruje on bibliotekę
CreatePHP_ (używaną przez bibliotekę `Create.js`_) z Symfony2 używając FOSRestBundle_.

Biblioteka Create.js działa przy wykorzystaniu warstwy REST. Wszystkie elementy
na stronie uzyskują `mapowanie RDFa`_, które powiadamia Create.js jak odwzorować
element do dokumentu. Podczas zapisu strony nowa treść przekazywana jest do API
REST i zapisywana w bazie danych.

Renderowanie treści z mapowanie RDFa może być bardzo proste, jak widać to w
Standard Edition:

.. code-block:: html+jinja

    {% block main %}
    {% createphp cmfMainContent as="rdf" %}
    {{ rdf|raw }}
    {% endcreatephp %}
    {% endblock %}

Wyświetli to obiekt zawartości z wykorzystaniem elementów `<div>`. Można to również
całkowicie przerobić używając funkcji ``createphp_*``.

BlockBundle
-----------

Jeśli odwiedzisz główną stronę Standard Edition, zobaczysz trzy bloki:

.. image:: ../_images/quick_tour/3rd-party-bundles-homepage.png

Bloki te można edytować i wykorzystywać zgodnie z własnymi potrzebami.
Są one dostarczane przez BlockBundle, który jest cienka warstwą na szczycie
SonataBlockBundle_. Zapewnia możliwość przechowywania bloków przy wykorzystaniu
PHPCR i dodaje niektóre powszechnie używane bloki.

Te trzy bloki na stronie głównej Standard Edition są blokami niestandardowymi.
Bloki są obsługiwane przez usługę bloków. Usługę tą można znaleźć w klasie 
``Acme\DemoBundle\Block\UnitBlockService``. Ponieważ bloki są utrwalane przy
użyciu PHPCR, to również potrzebny jest dokument bloku, który zlokalizowany jest
w ``Acme\DemoBundle\Document\UnitBlock``.

SeoBundle
---------

Istnieje również SeoBundle. Pakiet ten jest zbudowany na szczycie SonataSeoBundle_.
Dostarcza sposób na wyekstrahowanie informacji SEO z dokumentu i edytowania tych
danych przy użyciu interfejsu administracyjnego.

Dla zintegrowania SeoBundle w Standard Edition trzeba zarejestrować oba pakiety
w ``AppKernel``::

    // app/AppKernel.php

    // ...
    public function registerBundles()
    {
        $bundles = array(
            // ...
            new Sonata\SeoBundle\SonataSeoBundle(),
            new Symfony\Cmf\Bundle\SeoBundle\CmfSeoBundle(),
        );
        // ...
    }

Teraz można skonfigurować standardowy tytuł. Tytuł ten bedzie wykorzystywany podczas
wyodrębniania tytułu z obiektu treści przez CmfSeoBundle:

.. code-block:: yaml

    # app/config/config.yml
    cmf_seo:
        title: "%%content_title%% | Standard Edition"

Znacznik ``%%content_title%%`` zostanie zamieniony przez tytuł wyodrębniony z obiektu
treści. Ostatnia rzecza, którą trzeba zrobić, jest wykorzystanie tytułu w elemencie
*title*. Aby to zrobić, zamień linię znacznika ``<title>`` w szablonie
``src/Acme/DemoBundle/Resources/views/layout.html.twig`` na to:

.. code-block:: html+jinja

    {% block title %}{{ sonata_seo_title() }}{% endblock %}

Podczas odwiedzania każdej strony, będzie można zobaczyć na niej piękny tytuł.

Niektóre strony, jak strona logowania, nie wykorzystują obiektu treści. W takich
przypadkach można skonfigurować domyślny tytuł:

.. code-block:: yaml

    # app/config/config.yml
    sonata_seo:
        page:
            title: Standard Edition

.. caution::

    *Domyślny tytuł* skonfigurowany jest wewnątrz rozszerzenia ``sonata_seo``,
    natomiast *tytuł standardowy* wewnątrz rozszerzenia ``cmf_seo``.
    
Wyodrębnianie tytułu to tylko jedna z wielu możliwości SeoBundle – można wyodrębnić
przetworzyć więcej informacji SEO.

.. _quick-tour-third-party-sonata:

Interfejs administracyjny Sonata
--------------------------------

Wyjaśniliśmy, że  CMF został oparty na bazie danych, w celu uczynienia go możliwym
do edycji przez administratora bez zmieniania kodu. Lecz nie powiedzieliśmy jak
administrator będzie mógł zarządzać witryna. Teraz przyszedł czas aby odsłonić ten
sposób – wykorzystanie SonataAdminBundle_. Wszystkie pakiety CMF, które definiują
edytowalne elementy zapewniają również integrację z pakietem Sonata Admin, tak aby
można było elementy te edytować z poziomu interfejsu dostarczanego przez ten pakiet.

Domyślnie wszystkie klasy interfejsu administracyjnego w pakietach CMF są aktywowane
podczas instalacji SonataDoctrinePHPCRAdminBundle_. W konfiguracji można wyłączyć
klasę Adin. Na przykład, aby wyłączyć klasę Admin w MenuBundle trzeba zrobić tak:

.. code-block:: yaml

    # app/config/config.yml
    cmf_menu:
        persistence:
            phpcr:
                use_sonata_admin: false

Można również włączyć albo wyłączyć wszystkie klasy Admin CMF przez skonfigurowanie
tego w pakiecie ``cmf_core``:

.. code-block:: yaml

    # app/config/config.yml
    cmf_core:
        persistence:
            phpcr:
                use_sonata_admin: false

Gdy klasy Admin są aktywowane, administrator może przejść do ``/admin`` (jeśli
SonataAdminBundle został zainstalowany prawidłowo), Znajdzie tam dobrze znany
panel pulpitu administracyjnego, ze wszystkim co jest potrzebne:

.. image:: ../_images/quick_tour/3rd-party-bundles-sonata-admin.png

Jak widać po lewej stronie interfejsu administracyjnego wyświetlane jest, dzięki
pakietowi :doc:`TreeBrowserBundle <../bundles/tree_browser/introduction>`,
interaktywne drzewo administracyjne, w którym po kliknięciu węzła możliwa jest
jego edycja, usunięcie lub przesunięcie.

Wnioski końcowe
---------------

Dobrnęliśmy do końca. Podsumujmy czego nauczyliśmy się w takcie tego kursu:

* Symfony CMF powstał dla tworzenia systemów zarządzania treścią o wysokim stopniu
  dostosowywania;
* Zespół Symfony CMF tworzy pakiety o określonych funkcjach CMS, które mogą być
  użyte razem lub samodzielnie;
* Symfony CMF wykorzystuje bazę danych w celu umożliwienia administratorowi edytowania
  aplikacji bez konieczności zmiany kodu, jednakże konfiguracja jest przechowywana
  w systemie plików, aby umożliwić prostotę wdrożenia i obsługę kontroli wersji;
* Repozytorium treści PHP (ang. PHP Content Repository - PHPCR) jest wielką bazą
  danych zbudowana dla systemów CMS, ale można ją także wykorzystywać w każdym
  innym systemie magazynowania danych dla Symfony CMF;
* Zamiast wiązania kontrolerów z trasami, trasy są wiązane z obiektami treści.
* W Symfony CMF zadbano, aby „nie wyważać otwartych drzwi”. Integrowanych jest wiele
  pakietów powszechnie znanych w Symfony2.

Nie zdążyliśmy omówić architektury i wszystkich pakietów Symfony CMF,
ale jest to bardzo obszerny materiał. Zachęcamy do lektury
:doc:`Podręcznika <../book/index>` i do rozpoczęcia swojego pierwszego projektu
przy użyciu Symfony CMF.

.. _KnpMenuBundle: https://github.com/KnpLabs/KnpMenuBundle
.. _SonataBlockBundle: http://sonata-project.org/bundles/block/master/doc/index.html
.. _SonataSeoBundle: http://sonata-project.org/bundles/seo/master/doc/index.html
.. _CreatePHP: http://demo.createphp.org/
.. _`Create.js`: http://createjs.org/
.. _FOSRestBundle: https://github.com/friendsofsymfony/FOSRestBundle
.. _SonataAdminBundle: http://sonata-project.org/bundles/admin/master/doc/index.html
.. _SonataDoctrinePHPCRAdminBundle: http://sonata-project.org/bundles/doctrine-phpcr-admin/master/doc/index.html
.. _`mapowanie RDFa`: http://en.wikipedia.org/wiki/RDFa
