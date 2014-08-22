.. highlight:: php
   :linenothreshold: 2

Dokument witryny i strona domowa
--------------------------------

Wszystkie treści powinny być teraz dostępne pod różnymi adresami URL, ale strona
domowa (http://localhost:8000) wciąż pokazuje domyślną stronę indeksową Symfony
Standard Edition.

W tym rozdziale dodamy menu boczne do panelu administracyjnego Sonata, które umożliwia
użytkownikowi oznaczyć ``Page``, aby działała jako strona domowa CMS.

.. note::

    Jest to tylko jedna z wielu strategii możliwych dla stworzenia strony domowej.
    Na przykład innym rozwiazaniem może być umieszczenie dokumentu `RedirectRoute`
    w `/cms/routes`.

Przechowywanie danych
~~~~~~~~~~~~~~~~~~~~~

Potrzebny jest nam dokument, który przechowuje dane dotyczące CMS – nazywać go
będziemy dokumentem witryny i zawierać on będzie odniesienie do dokumentu ``Page``,
który będzie pełnił funkcję strony domowej.

Utwórzmy ten dokument witryny::

    // src/Acme/BasicCmsBundle/Document/Site.php
    namespace Acme\BasicCmsBundle\Document;

    use Doctrine\ODM\PHPCR\Mapping\Annotations as PHPCR;

    /**
     * @PHPCR\Document()
     */
    class Site
    {
        /**
         * @PHPCR\Id()
         */
        protected $id;

        /**
         * @PHPCR\ReferenceOne(targetDocument="Acme\BasicCmsBundle\Document\Page")
         */
        protected $homepage;

        public function getHomepage()
        {
            return $this->homepage;
        }

        public function setHomepage($homepage)
        {
            $this->homepage = $homepage;
        }
    }

Inicjowanie dokumentu witryny
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Gdzie znajduje sie dokument ``Site``? Hierarchia dokumentów wygląda tak:

.. code-block:: text
   :linenos:

    ROOT/
        cms/
           pages/
           routes/
           posts/

Jest jeden węzeł ``cms`` node, który zawiera wszystkie węzły podrzędne witryny.
Węzeł ten jest zatem logiczną pozycją dokumentu ``Site``.

Wcześniej użyliśmy ``GenericInitializer`` do inicjacji ścieżek bazowych naszego
projektu, w tym węzła ``cms``. Węzły utworzone przez ``GenericInitializer`` jednak
nie mają odwzorowania PHPCR-ODM.

Możemy *wymienić* ``GenericInitializer`` na własny inicjator, który będzie tworzył
niezbędne ścieżki i przypisywał klasę dokumentu do węzła ``cms``::

    // src/Acme/BasicCmsBundle/Initializer/SiteInitializer.php
    namespace Acme\BasicCmsBundle\Initializer;

    use Doctrine\Bundle\PHPCRBundle\Initializer\InitializerInterface;
    use PHPCR\Util\NodeHelper;
    use Doctrine\Bundle\PHPCRBundle\ManagerRegistry;

    class SiteInitializer implements InitializerInterface
    {
        private $basePath;

        public function __construct($basePath = '/cms')
        {
            $this->basePath = $basePath;
        }

        public function init(ManagerRegistry $registry)
        {
            $dm = $registry->getManagerForClass('Acme\BasicCmsBundle\Document\Site');
            if ($dm->find(null, $this->basePath)) {
                return;
            }

            $site = new Acme\BasicCmsBundle\Document\Site();
            $site->setId($this->basePath);
            $dm->persist($site);
            $dm->flush();

            $session = $registry->getConnection();

            // create the 'cms', 'pages', and 'posts' nodes
            NodeHelper::createPath($session, $this->basePath . '/pages');
            NodeHelper::createPath($session, $this->basePath . '/posts');
            NodeHelper::createPath($session, $this->basePath . '/routes');

            $session->save();
        }

        public function getName()
        {
            return 'My site initializer';
        }
    }

.. versionadded:: 1.1
    Począwszy od wersji 1.1, metoda ``init`` pobiera ``ManagerRegistry``
    zamiast ``SessionInterface`` PHPCR. Pozwala to na utworzenie dokumentów 
    w inicjatorach. W wersji 1.0, aby otrzymać prawidłową wartość, trzeba ręcznie
    ustawiać własność ``phpcr:class``.

Teraz zmodyfikujemy istniejącą konfigurację usługi dla ``GenericInitializer``,
jak poniżej:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # src/Acme/BasicCmsBundle/Resources/config/config.yml
        services:
            # ...
            acme_basiccms.phpcr.initializer.site:
                class: Acme\BasicCmsBundle\Initializer\SiteInitializer
                tags:
                    - { name: doctrine_phpcr.initializer }

    .. code-block:: xml
       :linenos:

        <!-- src/Acme/BasicCmsBUndle/Resources/config/config.php
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:acme_demo="http://www.example.com/symfony/schema/"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                 http://symfony.com/schema/dic/services/services-1.0.xsd">

            <!-- ... -->
            <services>
                <!-- ... -->
                <service id="acme_basiccms.phpcr.initializer.site"
                    class="Acme\BasicCmsBundle\Initializer\SiteInitializer">
                    <tag name="doctrine_phpcr.initializer"/>
                </service>
            </services>

        </container>

    .. code-block:: php
       :linenos:

        // src/Acme/BasicCmsBundle/Resources/config/config.php

        //  ...
        $container
            ->register(
                'acme_basiccms.phpcr.initializer.site',
                'Acme\BasicCmsBundle\Initializer\SiteInitializer'
            )
            ->addTag('doctrine_phpcr.initializer', array('name' => 'doctrine_phpcr.initializer')
        ;

Teraz opróżnimy repozytorium, aby go następnie zainicjować:

.. code-block:: bash

    $ php app/console doctrine:phpcr:node:remove /cms
    $ php app/console doctrine:phpcr:repository:init

i upewnimy się, czy węzeł ``cms`` został utworzymy prawidłowo, stosując polecenia
``doctrine:phpcr:node:dump`` z flagą ``props``:

.. code-block:: bash

    $ php app/console doctrine:phpcr:node:dump --props
    ROOT:
      cms:
        - jcr:primaryType = nt:unstructured
        - phpcr:class = Acme\BasicCmsBundle\Document\Site
        ...

.. note::

    Dlaczego warto korzystać z inicjatora zamiast konfiguratora testowania
    (*ang. fixtures*)? W tym przypadku, obiekt witryny (``Sites``) jest stały
    w aplikacji. Jest tylko jeden obiekt witryny, nie będą tworzone nowe witryny
    a istniejący dokument witryny nie będzie usunięty. DataFixtures mają dostarczyć
    przykładowe dane, a nie dane, które są integralną częścią witryny.

.. note::

    Zamiast *zamienić* ``GenericInitializer`` można po prostu dodać inny inicjator,
    który jest uruchamiany jako pierwszy i tworzy dokument ``/cms`` z właściwej klasy.
    Wadą jest to, że są dwa miejsca, w których dokonywany jest wybór inicjacji – rób
    więc jak wolisz.

Tworzenie przycisku wykonującego stronę domową
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Potrzebny jest sposób umożliwiający administratorowi witryny wybór strony, która
ma być strona domową. Można zmodyfikować klasę ``PageAdmin``, tak aby podczas
edytowania strony pojawiał się przycisk "Make Homepage". Osiągniemy to przez
dodanie "menu bocznego".

Po pierwsze, trzeba będzie stworzyć akcję, która będzie wykonywać działanie
przekształcające określoną stronę na stronę domową. Dodajmy następujący kod do
istniejącego kontrolera ``DefaultController``::

    // src/Acme/BasicCmsBundle/Controller/DefaultController.php

    // ...
    class DefaultController extends Controller
    {
        // ...

        /**
         * @Route(
         *   name="make_homepage",
         *   pattern="/_cms/make_homepage/{id}",
         *   requirements={"id": ".+"}
         * )
         */
        public function makeHomepageAction($id)
        {
            $dm = $this->get('doctrine_phpcr')->getManager();

            $site = $dm->find(null, '/cms');
            if (!$site) {
                throw $this->createNotFoundException('Could not find /cms document!');
            }

            $page = $dm->find(null, $id);

            $site->setHomepage($page);
            $dm->persist($page);
            $dm->flush();

            return $this->redirect($this->generateUrl('admin_acme_basiccms_page_edit', array(
                'id' => $page->getId()
            )));
        }
    }

.. note::

    Określiliśmy specjalne wymagania dla parametru``id`` trasy dlatego, że w domyślnych
    trasach nie można wstawiać z przodu znaku ukośnika ("/") w parametrach trasy
    a  nasz "id" jest ścieżką.

Teraz zmodyfikujemy klasę ``PageAdmin``, aby dodać przycisk w menu bocznym::

    // src/Acme/BasicCmsBundle/Admin/PageAdmin

    // ...
    use Knp\Menu\ItemInterface;
    use Sonata\AdminBundle\Admin\AdminInterface;

    class PageAdmin extends Admin
    {
        // ...
        protected function configureSideMenu(ItemInterface $menu, $action, AdminInterface $childAdmin = null)
        {
            if ('edit' !== $action) {
                return;
            }

            $page = $this->getSubject();

            $menu->addChild('make-homepage', array(
                'label' => 'Make Homepage',
                'attributes' => array('class' => 'btn'),
                'route' => 'make_homepage',
                'routeParameters' => array(
                    'id' => $page->getId(),
                ),
            ));
        }
    }

Są tu dwa argumenty nas interesujące:

* ``$menu``: będzie to pozycja menu głównego, do której można dodawać nowe elementy
  menu (jest to to samo API menu API z którym pracowaliśmy wcześniej);
* ``$action``: wskazuje na rodzaj strony konfigurowanej strony.

Jeśli ``edit`` nie jest akcją, to następuje wyjście z kodu i nie jest tworzone
jakiekolwiek menu boczne. Teraz, gdy już wiemy, że wymagane jest edytowanie strony,
to pobieramy *temat* z klasy administratora,która jest obecnie edytowanym obiektem
``Page``, co następnie dodaje element menu.

.. image:: ../../_images/cookbook/basic-cms-sonata-admin-make-homepage.png

Trasowanie strony domowej
~~~~~~~~~~~~~~~~~~~~~~~~~

Teraz, gdy już mamy włączonego administratora do wybierania strony, która będzie
używana jako strona domowa, musimy rzeczywiści doprowadzić to tego, aby CMS
wykorzystywał tą informację do renderowania wyznaczonej strony.

Można to łatwo zrobić, modyfikując akcję ``indexAction`` w ``DefaultController``,
w celu przekazywania żądań dopasowanych wzorca trasy ``/`` do akcji strony::

    // src/Acme/BasicCmsBundle/Controller/DefaultController.php

    // ...
    class DefaultController extends Controller
    {
        // ...

        /**
         * @Route("/")
         */
        public function indexAction()
        {
            $dm = $this->get('doctrine_phpcr')->getManager();
            $site = $dm->find('Acme\BasicCmsBundle\Document\Site', '/cms');
            $homepage = $site->getHomepage();

            if (!$homepage) {
                throw $this->createNotFoundException('No homepage configured');
            }

            return $this->forward('AcmeBasicCmsBundle:Default:page', array(
                'contentDocument' => $homepage
            ));
        }
    }

.. note::

    W przeciwieństwie do poprzednich przykładów, określiliśmy klasę podczas wywołania
    ``find``, a to dlatego że powinniśmy być pewni, że zwracany dokument jest klasy
    ``Site``.

W celu przetestowania tego, odwiedź http://localhost:8000.
