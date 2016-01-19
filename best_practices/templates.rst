.. highlight:: php
   :linenothreshold: 2

.. index::
   double: szablonowanie; najlepsze praktyki

Szablony
========

Gdy 20 lat temu został stworzony PHP, programiści cenili jego prostotę i to, jak
można dobrze mieszać HTML z kodem dynamicznym. Z biegiem czasu zaczęły powstawać
inne języki szablonowania, takie jak `Twig`_, które usprawniały szablonowanie.

.. best-practice::

    Do szablonowania używaj formatu Twig.

Ogólnie rzecz biorąc, szablony PHP są bardziej rozwlekłe, niż szablony Twig,
ponieważ brakuje im natywnego wsparcia dla wielu nowoczesnych funkcjonalności
potrzebnych w szablonach, takich jak dziedziczenie, automatyczne zabezpieczanie
danych wyjsciowych, czy stosowanie nazwanych argumentów dla filtrów i funkcji.

Twig jest w Symfony domyślnym formatem szablonowania i jest wykorzystywany
w wielu projektach, m.in. w Drupal 8.

Ponadto, Twig jest jedynym formatem szablonowania, posiadającym gwarancję wsparcia
w Symfony 3. W rzeczywistości, PHP może zostać usunięty w oficjalnie wspieranych
silnikach szablonowania.

Umiejscawienie szablonów
------------------------

.. best-practice::

    Przechowuj wszystkie szablony aplikacji w katalogu ``app/Resources/views/``.

Zwykle, programiści Symfony przechowują szablony aplikacji w katalogu
``Resources/views/`` każdego pakietu i dla odniesienia się do tych szablonów,
wykorzystują nazwę logiczną (np. ``AcmeDemoBundle:Default:index.html.twig``).

Szablony używane w aplikacji wygodniej jest przechowywać w katalogu
``app/Resources/views/``. Powoduje to znaczne uproszczenie ich nazwy logicznej:

=================================================  ==================================
Szablony przechowywane w pakietach                 Szablony przechowywane w ``app/``
=================================================  ==================================
``AcmeDemoBundle:Default:index.html.twig``         ``default/index.html.twig``
``::layout.html.twig``                             ``layout.html.twig``
``AcmeDemoBundle::index.html.twig``                ``index.html.twig``
``AcmeDemoBundle:Default:subdir/index.html.twig``  ``default/subdir/index.html.twig``
``AcmeDemoBundle:Default/subdir:index.html.twig``  ``default/subdir/index.html.twig``
=================================================  ==================================

Kolejna zaletą jest scentralizowanie szablonów, co upraszcza pracę projektantom
wyglądu. Nie muszą oni wyszukiwać szablonów w wielu katalogach, rozrzuconych po
różnych pakietach.

.. best-practice::

    Stosuj nazwy katalogów i szablonów pisane małymi literami, z rozdzielaniem
    wyrazów znakami podkreślenia.

Rozszerzenia Twig
-----------------

.. best-practice::

    Definiuj rozszerzenia Twig w katalogu ``AppBundle/Twig/`` i konfiguruj je,
    stosując plik ``app/config/services.yml``.

Przyjmijmy, że nasza aplikacja potrzebuje własnego filtra Twig ``md2html``,
tak aby można było przekształcać treści poszczególnych wpisów w formacie Markdown
na HTML. W tym celu zainstalujemy najpierw doskonały parser Markdown o nazwie
`Parsedown`_ jako nową zależność projektu:

.. code-block:: bash

    $ composer require erusev/parsedown

Następnie, utworzymy nową usługę ``Markdown``, która będzie później używana
przez rozszerzenie Twig. Definicja usługi wymaga podania tylko ścieżki do klasy:

.. code-block:: yaml
   :linenos:

    # app/config/services.yml
    services:
        # ...
        markdown:
            class: AppBundle\Utils\Markdown

W klasie ``Markdown`` po prostu musimy zdefiniować jedną metodę do
przetransformowania treści Markdown na HTML::

    namespace AppBundle\Utils;

    class Markdown
    {
        private $parser;

        public function __construct()
        {
            $this->parser = new \Parsedown();
        }

        public function toHtml($text)
        {
            $html = $this->parser->text($text);

            return $html;
        }
    }

Następnie, tworzymy nowe rozszerzenie Twig i definiujemy nowy filtr o nazwie
``md2html``, wykorzystujac klasę ``Twig_SimpleFilter``. Po czym, wstrzykujemy
nowo utworzoną usługę ``markdown`` do konstruktora rozszerzenia Twig:

.. code-block:: php
   :linenos:

    namespace AppBundle\Twig;

    use AppBundle\Utils\Markdown;

    class AppExtension extends \Twig_Extension
    {
        private $parser;

        public function __construct(Markdown $parser)
        {
            $this->parser = $parser;
        }

        public function getFilters()
        {
            return array(
                new \Twig_SimpleFilter(
                    'md2html',
                    array($this, 'markdownToHtml'),
                    array('is_safe' => array('html'))
                ),
            );
        }

        public function markdownToHtml($content)
        {
            return $this->parser->toHtml($content);
        }

        public function getName()
        {
            return 'app_extension';
        }
    }

Na koniec, definiujemy nową usługę, umożliwiajacą stosowanie rozszerzenia Twig
w aplikacji (nazwa tej usługi nie ma znaczenia, ponieważ nie bedziemy jej nigdy
używać we własnym kodzie):

.. code-block:: yaml
   :linenos:

    # app/config/services.yml
    services:
        app.twig.app_extension:
            class:     AppBundle\Twig\AppExtension
            arguments: ['@markdown']
            public:    false
            tags:
                - { name: twig.extension }

.. _`Twig`: http://twig.sensiolabs.org/
.. _`Parsedown`: http://parsedown.org/
