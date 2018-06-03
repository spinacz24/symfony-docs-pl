.. highlight:: php
   :linenothreshold: 2

Przegląd
========

Zacznij korzystać z Symfony w 10 minut! Ten rozdział wyjaśnia kilka
najważniejszych pojęć Symfony, oraz pozwala szybko rozpocząć
pracę, demonstrując prosty przykład aplikacji.

Jeśli Czytelnik kiedykolwiek używał frameworka aplikacji internetowej, to z Symfony
powinien czuć się jak w domu.
Jeśli nie, to zapraszamy do poznania zupełnie nowego sposobu tworzenia aplikacji
internetowych.

.. _installing-symfony2:

Instalowanie Symfony
--------------------

Przed dalszą lekturą tego rozdziału, upewnij się że masz zaistalowane PHP jak
i Symfony, tak jak wyjaśniono to w :doc:`rozdziale "Instalacja" </book/installation>`
podręcznika Symfony.

Podstawy
--------

Jednym z głównych celów frameworka (platformy aplikacyjnej) jest dobre zorganizowanie
kodu i umożliwienie łatwego rozwoju aplikacji, z uniknięciem mieszania w jednym
skrypcie wywołań bazy danych, znaczników HTML i logiki biznesowej. W celu zrozumienia,
jak to działa w Symfony, najpierw musisz poznać kilka podstawowych pojęć i terminów.

Podczas tworzenia aplikacji Symfony, rola programisty polaga na napisaniu kodu,
który odwzorowuje *żądanie* użytkownika (np.  ``http://localhost:8000/``)
na *zasób* związany z tym żądaniem (stron HTML ``Homepage``).

Kod, który ma być wykonany, jest zdefiniowany w **akcjach** i **kontrolerach**.
Odwzorowanie pomiędzy żądaniem użytkownika a tym kodem jest zdefiniowane w
konfiguracji **trasowania** .
Treści wyświetlane w przeglądarce są zazwyczaj renderowane przy użyciu **szablonów**.

Kiedy wywołało się ``http://localhost:8000/app/example``, Symfony wykonał kod
kontrolera, zdefiniowany w pliku ``src/AppBundle/Controller/DefaultController.php``
i zrenderował szablon ``app/Resources/views/default/index.html.twig``.
W następnym rozdziale dowiesz się o szczegółach wewnętrznego funkcjonowania kontrolerów
Symfony, trasach i szablonach.

Akcje i kontrolery
~~~~~~~~~~~~~~~~~~

Otwórz plik ``src/AppBundle/Controller/DefaultController.php`` i obejrzyj zawarty
tam kod (na razie nie będziemy zajmować się konfiguracją ``@Route``, ponieważ
zostanie to wyjaśnione w następnym rozdziale)::

    namespace AppBundle\Controller;

    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;
    use Symfony\Bundle\FrameworkBundle\Controller\Controller;

    class DefaultController extends Controller
    {
        /**
         * @Route("/", name="homepage")
         */
        public function indexAction()
        {
            return $this->render('default/index.html.twig');
        }
    }

W aplikacji Symfony, **kontrolery**, to klasy PHP, których nazwy są
zakończone słowem ``Controller``. W tym przykładzie kontroler nosi nazwę
``Default`` a klasa PHP ma nazwę ``DefaultController``.

Metody zdefiniowane w kontrolerze są nazywane **akcjami** - zwykle związane są
z jakimś jedym adresem URL aplikacji i ich nazwy kończą się słowem ``Action``.
W naszym przykładzie kontroler ``Default`` ma tylko jedną akcję o nazwie ``index``
i definiuje metodę ``indexAction``.

.. note::
   W niniejszym podręczniku, można się spotkać (jeszcze) ze stosowaniem słowa
   *kontroler* w znaczeniu *akcja*, co zostało przeniesione z dokumentacji
   anglojezycznej. Np. mówiąc, że "trasa odwzorowuje żądanie użytkownika na kontroler",
   trzeba to rozumieć, że "trasa odwzorowuje żądanie użytkownika na odpowiednią
   metodę kontrolera, czyli na akcję. Tak więc, czytajac o "kontrolerze" proszę
   zwracać uwagę na kontekst.

Kod akcji jest zazwyczaj bardzo krótki - około 10-15 linii kodu - ponieważ akcje
odwołują się do innych części aplikacji, w celu pobrania lub wygenerowania
potrzebnych informacji i renderowania szablonu, aby ostatecznie pokazać wynik
użytkownikowi.

W tym przykładzie akcja ``index`` jest praktycznie pusta, ponieważ nie ma potrzeby
wywoływania innych metod. Akcja ta tylko renderuje szablon z treścią *Homepage*.

Trasowanie
~~~~~~~~~~

System trasowania (*ang. routing*), nazywany też w polskiej literaturze "systemem
przekierowań", w Symfony obsługuje żądania klienta, dopasowując ścieżkę dostępu
(zawartą w adresie URL) do skonfigurowanych wzorców tras i przekazuje sterowanie
właściwej akcji.
Otwórzmy ponownie plik ``src/AppBundle/Controller/DefaultController.php`` i skupmy
się na trzech pierwszych liniach metody ``indexAction``::

   // src/AppBundle/Controller/DefaultController.php
    namespace AppBundle\Controller;

    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;
    use Symfony\Bundle\FrameworkBundle\Controller\Controller;

    class DefaultController extends Controller
    {
        /**
         * @Route("/", name="homepage")
         */
        public function indexAction()
        {
            return $this->render('default/index.html.twig');
        }
    }

Te trzy pierwsze linie definiują konfigurację trasowania przy użyciu adnotacji
``@Route()``. **Adnotacja PHP** jest wygodnym sposobem konfigurowania metody bez
konieczności pisania zwykłego kodu PHP. Trzeba pamiętać, że bloki adnotacji
rozpoczynają się od ``/**``, natomiast zwykłe komentarze od ``/*``.

Pierwsza wartość adnoacji ``@Route()`` określa adres URL, która spowoduje wykonanie
określonej akcji. ponieważ nie trzeba dodawać schematu i hosta z adresu URL
(np. ``http://example.com``), te adresy URL są zawsze względne i nazywamy je
*ściezkami*. W naszym przypadku, ścieżka ``/`` odnosi się do aplikacji
homepage. Druga wartość adnotacji ``@Route()`` (tj. ``name="homepage"``) jest
opcjonalna i ustawia nazwę tej trasy. Na razie ta nazwa jest nieprzydatna, ale
później stanie się potrzebna do linkowania stron.

Uwzględniajac to wszystko, adnotacja ``@Route("/", name="homepage")``
tworzy nową trasę o nazwie ``homepage``, co powoduje, że Symfony wykonuje akcję
``index`` kontrolera ``Default``, gdy użytkownik odwiedzi adres URL aplikacji ze
ścieżką ``/``.

.. tip::

    Oprócz adnotacji PHP, trasy można konfigurować w plikach YAML, XML lub
    PHP, tak jak wyjaśniono to w rozdziale :doc:`Trasowanie </book/routing>`
    podręcznika Symfony. ta elastyczność jest jedną z głównych cech frameworka
    Symfony, który nigdy nie narzuca konkrenego formatu konfiguracji.

Szablony
~~~~~~~~

Jedyną zawartością akcji ``index`` jest instrukcja PHP::

    return $this->render('default/index.html.twig');

Metoda ``$this->render()`` jest wygodnym skrótem renderującym szablon.
Symfony dostarcza kilka przydatnych skrótów do każdego kontrolera rozszerzającego
klasę ``Controller``.

Domyślnie, szablony aplikacji są przechowywane w katalogu ``app/Resources/views/``.
Dlatego szablon ``default/index.html.twig``, to to samo co
``app/Resources/views/default/index.html.twig``. Otwórz ten plik i przyjrzyj się
temu kodowi:

.. code-block:: html+jinja
   :linenos:

    {# app/Resources/views/default/index.html.twig #}
    {% extends 'base.html.twig' %}

    {% block body %}
        <h1>Witamy w Symfony</h1>
    {% endblock %}

Szablon ten jest utworzony w `Twig`_, nowym silniku szablonowania, przeznaczonym
dla nowoczesnych aplikacji PHP.
:doc:`Druga część tego poradnika </quick_tour/the_view>` wyjaśni Ci, jak działają
szablony w Symfony.

.. _quick-tour-big-picture-environments:

Praca ze środowiskami
---------------------

Teraz, gdy już lepiej rozumiemy działanie Symfony, przyjrzymy się bliżej stopce
renderowanej na każdej stronie Symfony. Możesz tam zauważyć mały pasek z logo Symfony.
Jest on nazywany "paskiem debugowania" (*ang. "Web Debug Toolbar"*) i jest to najlepszy
przyjaciel programisty.

.. image:: /images/quick_tour/web_debug_toolbar.png
   :align: center

To co teraz można zobaczyć, jest tylko „wierzchołkiem góry lodowej”.
Klikniecie na jakąkolwiek sekcję paska otworzy profiler i będzie można uzyskać
znacznie więcej informacji o żądaniu, parametrach zapytania, szczegółach zabezpieczeń
i kwerendach bazy danych:

.. image:: /images/quick_tour/profiler.png
   :align: center

Narzędzie to dostrcza bardzo dużo wewnetrznej informacji o aplikacji, dlatego z
pewnością nie można jej pokazywać publicznie. Symfony nie pokazuje tego paska
narzędziowego, gdy aplikacja jest uruchamiana na serwerze produkcyjnym w "trybie
publicznym".

Skąd Symfony wie, czy aplikacja ma być uruchomiona w "trybie publicznym"? Dowiesz
się tego czytając o pojeciu **środowisko wykonawcze**.

.. _quick-tour-big-picture-environments-intro:

Co to jest środowisko?
~~~~~~~~~~~~~~~~~~~~~~

:term:`Środowisko <środowisko>` reprezentuje grupę konfiguracji wykorzystywanych
podczas uruchamiania aplikacji. Symfony definiuje domyślnie dwa środowiska:
``dev`` (wykorzystywane lokalnie przy pracach programistycznych nad aplikacją)
i ``prod`` (zoptymalizowane dla wykonywania aplikacji na serwerze produkcyjnym).

Gdy odwiedza się w przeglądarce adres ``http://localhost:8000``, wykonuje się
aplikację Symfony w środowisku ``dev``. W celu odwiedzenia tej aplikacji w środowisku
``prod``, trzeba zamiast tego odwiedzić adres ``http://localhost:8000/app.php``.
Jeśli chce się, aby zawsze środowisko ``dev`` było pokazywane w adresie URL,
trzeba odwiedzić adres ``http://localhost:8000/app_dev.php``.

Zasadniczą różnicą pomiędzy tymi środowiskami jest to, że środowisko ``dev`` jest
zoptymalizowane pod względem dostarczania dużej ilości informacji dla programisty,
co oznacza gorszą wydajność. Natomiast środowisko ``prod`` jest zoptymalizowane
pod kątem jak największej wydajności, co oznacza, że debugowanie nie jest realizowane
i niedostępna jest tego typu informacja, jak też cały pasek debugowania.

Druga różnica między środowskami polega na używaniu przez aplikację innych opcji
konfiguracyjnych w poszczególnych środowiskach. Gdy ma się dostęp do środowiska
``dev``, Symfony ładuje plik konfiguracyjny ``app/config/config_dev.yml``,
a w środowisku ``prod``, plik ``app/config/config_prod.yml``.

Zazwyczaj, środowiska udostępniają dużą ilość opcji konfiguracyjnych. Z tego powodu,
wspólne opcje konfiguracyjne wstawiane są do wspólnego pliku ``config.yml``
i nadpisywane w razie potrzeby przez opcje umieszczane w pliku konfiguracyjnym
specyficznym dla środowiska:

.. code-block:: yaml
   :linenos:

    # app/config/config_dev.yml
    imports:
        - { resource: config.yml }

    web_profiler:
        toolbar: true
        intercept_redirects: false

W tym przykładzie, środowisko ``dev`` ładuje plik konfiguracyjny ``config_dev.yml``,
który sam importuje wspólny plik ``config.yml`` i modyfikuje go, udostępniając
pasek narzędziowy debugowania.

Po odwiedzeniu w przegladarce pliku``app_dev.php``, wykonuje się aplikację Symfony
w środowisku ``dev``. W celu uruchomienia aplikacji w środowisku ``prod``, trzeba
natomiast odwiedzić plik ``app.php``.

Więcej szczegółów o środowiskach można znaleźć w artykule
":ref:`Środowidka i kontroler wejścia <page-creation-environments>`".

Podsumowanie
------------

Gratulacje! Miałeś Czytelniku przedsmak kodowania Symfony. To nie było tak trudne, prawda?
Jest dużo więcej do odkrycia, ale teraz trzeba zobaczyć, jak Symfony sprawia,
że ​​naprawdę łatwo jest wdrożyć strony internetowe. Jeśli chcesz się dowiedzieć
więcej o Symfony, zacznij lekturę następnej części przewodnika: ":doc:`the_view`.

.. _Composer:             https://getcomposer.org/
.. _executable installer: http://getcomposer.org/download
.. _Twig:                 http://twig.sensiolabs.org/
