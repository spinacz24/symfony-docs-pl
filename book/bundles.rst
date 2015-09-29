.. index::
   single: pakiety

.. _page-creation-bundles:

System pakietów
===============

Pakiety (*ang. bundles*) są podobne do wtyczek w innych programach, ale sa od
nich lepsze. Zasadnicza różnica polega na tym, że całe Symfony jest zbudowane
z pakietów, łącznie z rdzeniem frameworka i kodem, który piszesz dla aplikacji.
W Symfony, pakiety są "obywatelem numer jeden". Zapewnia to elastyczność we
wstępnym budowaniu funkcjonalności w oparciu o `pakiety osób trzecich`_ lub
w dystrybucji własnych pakietów. Sprawia to łatwość w doborze funkcjonalności,
które trzeba udostępnić w aplikacji i umożliwia ich zoptymalizowanie w sposób,
jaki się chce.

.. note::

   Po przeczytaniu tego rozdziału i poznaniu podstaw będzie można siegnąć po
   lekturę artykułów
   :doc:`w Receptariuszu, poświeconych pakietom </cookbook/bundles/best_practices>`,
   które omawiają zagadnienia związane z prganizacją i najlepszymi praktykami
   w tym zakresie.

Pakiet jest ustrukturyzowanym zestawem plików w odrębnym katalagu, implementujacych
jakąś pojedynczą funkcjonalność. Można utworzyć BlogBundle, ForumBundle lub pakiet
do zarządzania użytkownikami (wiele z nich już istnieje jako pakiety osób trzecich).
Każdy katalog zawiera wszystko, co zwiazane jest z dana funkcjonalnością, włączając
w to pliki PHP, szablony, arkusze stylów, pliki JavaScript, testy itd..
Kazdy aspekt funkcjonalności istnieje w pakiecie i każda funkcjonalność umieszczona
jest w pakiecie.

Pakiety po ich zainstalowaniu trzeba włączyć, rejestrując je w metodzie
``registerBundles()`` klasy ``AppKernel``::

    // app/AppKernel.php
    public function registerBundles()
    {
        $bundles = array(
            new Symfony\Bundle\FrameworkBundle\FrameworkBundle(),
            new Symfony\Bundle\SecurityBundle\SecurityBundle(),
            new Symfony\Bundle\TwigBundle\TwigBundle(),
            new Symfony\Bundle\MonologBundle\MonologBundle(),
            new Symfony\Bundle\SwiftmailerBundle\SwiftmailerBundle(),
            new Symfony\Bundle\DoctrineBundle\DoctrineBundle(),
            new Symfony\Bundle\AsseticBundle\AsseticBundle(),
            new Sensio\Bundle\FrameworkExtraBundle\SensioFrameworkExtraBundle(),
            new AppBundle\AppBundle(),
        );

        if (in_array($this->getEnvironment(), array('dev', 'test'))) {
            $bundles[] = new Symfony\Bundle\WebProfilerBundle\WebProfilerBundle();
            $bundles[] = new Sensio\Bundle\DistributionBundle\SensioDistributionBundle();
            $bundles[] = new Sensio\Bundle\GeneratorBundle\SensioGeneratorBundle();
        }

        return $bundles;
    }

Przy pomocy metody ``registerBundles()`` ma się całkowitą kontrolę nad tym, które
pakiety sa używane w aplikacji (łącznie z pakietami rdzenia Symfony).

.. tip::

   Pakiet może zostać umieszczony *gdziekolwiek* pod warunkiem, że będą dostępne
   dla autoloadera skonfigurowanego w ``app/autoload.php``.

Tworzenie pakietu
-----------------

Symfony Standard Edition dostarczane jest z wygodnym zadaniem, które tworzy w pełni
funkcjonalny pakiet do wykorzystania przez programistę. Oczywiście, ręczne tworzenie
oakietu jest równie proste.

W celu pokazania, jak łatwo jest stworzyć system pakietu utworzymy nowy pakiet
o nazwie AcmeTestBundle i go włączymy.

.. tip::

    Porcja ``Acme`` jest po prostu testową nazwą, która powinna zostać zamieniona
    na jakąś nazwę "dostawcy", reprezentującą Ciebie lub organizację (np.
    ABCTestBundle dla dostawcy o nazwie ``ABC``).

Rozpocznijmy od utworzenia katalogu ``src/Acme/TestBundle/`` i dodamy tam nowy
plik o nazwie ``AcmeTestBundle.php``::

    // src/Acme/TestBundle/AcmeTestBundle.php
    namespace Acme\TestBundle;

    use Symfony\Component\HttpKernel\Bundle\Bundle;

    class AcmeTestBundle extends Bundle
    {
    }

.. tip::

   Nazwa AcmeTestBundle spełnia standard
   :ref:`konwencji nazewniczej pakietów <bundles-naming-conventions>`.
   Można również skrócic nazwą pakietu, do TestBundle, przez nazwanie klasy
   TestBundle (i nazanie plu klasy ``TestBundle.php``).

Ta pusta klasa jest tylko cząstką potrzebną do stworzenia nowego pakietu.
Chociaż jest ona pusta, to jest pełno wartosciowa i może być zastosowana do
dostosowaia zachowania pakietu.

Teraz, gdy został już utworzony pakiet, trzeba go włączyć w klasie ``AppKernel``::

    // app/AppKernel.php
    public function registerBundles()
    {
        $bundles = array(
            // ...
            // register your bundle
            new Acme\TestBundle\AcmeTestBundle(),
        );
        // ...

        return $bundles;
    }

Chociaż pakie AcmeTestBundle nie robi na razie nic, jest gotowy do użycia.

Symfony dostarcza również interfejs linii poleceń dla generowania podstawowego
szkieletu pakietu:

.. code-block:: bash

    $ php app/console generate:bundle --namespace=Acme/TestBundle

Szkielet pakietu generuje podstawowy kontroler, szablon i źródło trasowania, ktore
mozna dostosować. Później nauczymy sie więcej o narzędziach linii poleceń.

.. tip::

   Podczas ręcznego tworzenia własnego pakietu lub zainstalowania pakietu zewnętrznego
   trzeba się upewnić, że pakiet został włączony w metodzie ``registerBundles()``.
   Gdy pakiet generuje się poleceniem ``generate:bundle``, to ten krok jest
   wykonywany automatycznie.

Struktura katalogu pakietu
--------------------------

Struktura katalogowa pakietu jest prosta i elastyczna. Domyślnie, system pakietów
zgodne jest z konwencją, która pomaga zachować zgodność kodu pomiędzy różnymi
pakietami Symfony. Przyjrzymy się strukturze AcmeDemoBundle, ponieważ zawiera on
pewne najbardziej typowe elementy pakietu:

``Controller/``
    Zawiera kontrolery pakietu (np. ``RandomController.php``).

``DependencyInjection/``
    Posiada pewne klasy Dependency Injection Extension, które mogą importować
    konfigurację usługi, rejestrować bilety kompilera i więcej (katalog ten nie
    jest obowiazkowy).

``Resources/config/``
    Przechowuje konfigurację, w tym konfigurację trasowania (np. ``routing.yml``).

``Resources/views/``
    Zawiera szablony organizowane zgodnie z nazwami akcji (np. ``Hello/index.html.twig``).

``Resources/public/``
    Zawiera aktywa internetow (obrazy, arkusze stylów itd.) i jest kopiowany lub
    odnoszony symbolicznie do katalogu ``web/`` projektu poprzez polecenie konsolowe
    ``assets:install``.

``Tests/``
    Przechowuje wszystkie testy dla pakietu.

Pakiet może być mały lub duży, w zależności od funkcjonalności jaka implementuje.
Zawiera tylko potrzebne pliki i nic ponad to.

W dalszej lekturze podręcznia dowiesz się jak utrwalac obiekty w bazie danych,
tworzyć i walidować formularze, tworzyć tłumaczenia dla aplikacji, pisać testy
i więcej. Każdy z tych elementów ma swoje miejsce i role w pakiecie.

_`pakiety osób trzecich`: http://knpbundles.com
