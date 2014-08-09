.. index::
    single: rdzeń CMF; pakiety; proces publikowania
    single: elektory publikowania
    single: procedura kontroli publikacji

Proces publikowania
-------------------

System procesu publikowania (*ang. publish workflow system*) pozwala kontrolować
to, jaka treść jest dostępna na witrynie. Jest to podobne do
`komponentu bezpieczeństwa Symfony2`_.
Jednak w przeciwieństwie do kontekstu bezpieczeństwa, procedura kontroli publikacji
może być realizowane nawet, gdy nie jest umieszczona żadna zapora i zatem kontekst
bezpieczeństwa nie ma żadnego tokenu (zobacz `Uwierzytelnianie w Symfony2`_).

Proces publikowania jest również związany z procesem bezpieczeństwa:
pakiet CoreBundle rejestruje **elektory** (obiekty *voters*), które
przekazują weryfikację bezpieczeństwa do procesu publikowania. Oznacza to, że
jeśli zawsze ma się zaporę, można po prostu użyć zwykły kontekst bezpieczeństwa
oraz funkcję ``is_granted`` Twig, aby sprawdzić publikacje.

.. tip::

    Dobrym wprowadzeniem do rdzennych mechanizmów bezpieczeństwa Symfony jest
    rozdział :doc:`"System bezpieczeństwa" </book/security>` podręcznika.


Domyślny proces publikowania odpowiada następującemu diagramowi:

.. image:: ../../_images/bundles/core_pwf_workflow.png

Zwracane wartości przez ``getPublishStartDate`` i ``getPublishEndDate`` mogą wynosić
``null``. W takim przypadku data początkowa i końcowa jest nieograniczona.
Na przykład, jeśli data końcowa to ``null`` a data początkowa wynosi ``2013-09-29``,
to obiekt będzie publikowany od daty początkowej i nigdy nie zostanie wycofany
z publikacji ("unpublished").

Sprawdzenie czy treść jest opublikowana
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Pakiet udostępnia usługę ``cmf_core.publish_workflow.checker``, która implementuje
klasę :class:`Symfony\\Component\\Security\\Core\\SecurityContextInterface`
komponentu bezpieczeństwa Symfony2. Metodą procedyry kontroli publikacji jest 
:method:`Symfony\\Component\\Security\\Core\\SecurityContextInterface::isGranted`,
ta sama, co w kontekście bezpieczeństwa.

Metoda ta jest używana też podczas wykonywania `procedur kontrolnych ACL`_: pierwszym
argumentem jest pożądana akcja, drugim obiekt treści, na którym chce się wykonać
akcję.

Obecnie jedynymi akcjami obsługiwanymi przez elektory są ``VIEW``
i ``VIEW_ANONYMOUS``. Posiadanie prawa do wglądu oznacza, że bieżącemu użytkownikowi
wolno oglądnąć tą treść, dlatego że jest opublikowana albo z powodu jego specyficznych
uprawnień. W niektórych kontekstach aplikacja może nie chcieć pokazywać niepublikowaną
treść, nawet uprzywilejowanym użytkownikom, tak aby ich nie mylić. W tym celu używane
są uprawnienia "view anonymous".

Procedura kontroli publikacji (*ang. workflow checker*) jest konfigurowana z rolą,
która umożliwia ominięcie sprawdzania publikacji, tak że procedura ta widzi
niepublikowaną treść. Rola ta powinna być ustalona na poziomie redaktorów. Domyślna
nazwa tej roli to ``ROLE_CAN_VIEW_NON_PUBLISHED``.

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            role_hierarchy:
                ROLE_EDITOR: ROLE_CAN_VIEW_NON_PUBLISHED

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:srv="http://symfony.com/schema/dic/services">

            <config>
                <role id="ROLE_EDITOR">ROLE_CAN_VIEW_NON_PUBLISHED</role>
            </config>

        </srv:container>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'role_hierarchy' => array(
                'ROLE_EDITOR' => 'ROLE_CAN_VIEW_NON_PUBLISHED',
            ),
        ));

Gdy jest zalogowany użytkownik z rolą ``ROLE_EDITOR``, oznacza to, że w zapytaniu
ścieżki umieszczona jest zapora i użytkownik ten będzie miał uprawnienia do oglądania
widoków niepublikowanych treści::

    use Symfony\Cmf\Bundle\CoreBundle\PublishWorkflow\PublishWorkflowChecker;

    // check if current user is allowed to see this document
    $publishWorkflowChecker = $container->get('cmf_core.publish_workflow.checker');
    if ($publishWorkflowChecker->isGranted(
            PublishWorkflowChecker::VIEW_ATTRIBUTE,
            $document
        )
    ) {
        // ...
    }

    // check if the document is published. even if the current role would allow
    // to see the document, this will still return false if the documet is not
    // published
    if ($publishWorkflowChecker->isGranted(
            PublishWorkflowChecker::VIEW_ANONYMOUS_ATTRIBUTE,
            $document
        )
    ) {
        // ...
    }

.. _bundle-core-publish-workflow-twig_function:

W celu sprawdzenia publikacji w szablonie, trzeba użyć funkcji Twiga ``cmf_is_published``
lub metody ``$view['cmf']->isPublished``:

.. configuration-block::

    .. code-block:: jinja

        {# check if document is published, regardless of current users role #}
        {% if cmf_is_published(page) %}
            {# ... output the document #}
        {% endif %}

        {#
            check if current logged in user is allowed to view the document either
            because it is published or because the current user may view unpublished
            documents.
        #}
        {% if is_granted('VIEW', page) %}
            {# ... output the document #}
        {% endif %}

    .. code-block:: html+php

        <!-- check if document is published, regardless of current users role -->
        <?php if ($view['cmf']->isPublished($page)) : ?>
            <!-- ... output the document -->
        <?php endif ?>

        <!--
            check if current logged in user is allowed to view the document either
            because it is published or because the current user may view unpublished
            documents.
        -->
        <?php if ($view['security']->isGranted('VIEW', $page)) : ?>
            <!-- ... output the document -->
        <?php endif ?>

.. note::

    Rozdział :doc:`”Szablonowanie” <templating>` objaśnia wszystkie funkcje
    pomocnicze szablonowania dostarczane przez CMF. Helpery te już wykorzystują
    proces publikowania.

Kod ładujący treść powinien wykonywać procedurę kontroli publikacji. Dzięki
:ref:`odbiornikowi żądań <bundle-core-workflow-request_listener>` trasy i główna
treść dostarczana przez :doc:`DynamicRouter <../routing/dynamic>` są również
sprawdzane automatycznie.

Możliwe jest jawne ustawienie tokena bezpieczeństwa w procedurze kontroli publikacji.
Jednak domyślnie, procedura taka nabędzie token z domyślnego kontekstu bezpieczeństwa,
a gdy go nie ma (zwykle, gdy brak jest zapory w ścieżce URL), to w locie jest tworzony
obiekt :class:`Symfony\\Component\\Security\\Core\\Authentication\\Token\\AnonymousToken`.

Jeśli sprawdza się ``VIEW`` a nie ``VIEW_ANONYMOUS``, to najpierw sprawdzone zostanie,
czy kontekst bezpieczeństwa zna bieżącego użytkownika i czy temu użytkownikowi
przydzielona jest rola omijająca. Jeśli tak, to udzielany jest dostęp, w przeciwnym
razie zostaje delegowana klasa
:class:`Symfony\\Component\\Security\\Core\\Authorization\\AccessDecisionManager`,
która wywołuje wszystkie elektory z żądanymi atrybutami, obiekt i token.

Menadżer decyzji jest skonfigurowany dla zgodnych głosowań wg reguły
"zezwól, jeśli wszyscy się wstrzymują". Oznacza to, że gdy tylko jeden elektor
powie ``ACCESS_DENIED``, będzie to wystarczające do uznania treści za nieopublikowaną.
Jeśli wszystkie elektory nie dadzą głosu (na przykład, gdy
treść w pytaniu nie implementuje jakiejkolwiek funkcjonalności procesu publikowania)
treść jest nadal uważana za publikowaną.

Podporządkowywanie dokumentów procesowi publikowania
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Komponent procesu publikacji posiada 4 interfejsy:
``PublishableInterface``, ``PublishTimePeriodInterface`` i odpowiadające
im interfejsy tylko do odczytu.

.. image:: ../../_images/bundles/core_pwf_interfaces.png

Używanie interfejsów tylko do  odczytu podczas modyfikowania informacji nie jest zalecane.

Poniżej znajduje się przykład implementacji procesu publikowania::

    namespace Acme\BlogBundle\Document;

    use Symfony\Cmf\Bundle\CoreBundle\PublishWorkflow\PublishableInterface;
    use Symfony\Cmf\Bundle\CoreBundle\PublishWorkflow\PublishTimePeriodInterface;

    class Post implements PublishableInterface, PublishTimePeriodInterface
    {
        // ... properties and methods

        /**
         * @var \DateTime
         */
        protected $publishStartDate;

        /**
         * @var \DateTime
         */
        protected $publishEndDate;

        /**
         * @var boolean
         */
        protected $isPublishable;

        public function setPublishStartDate(\DateTime $startDate = null)
        {
            $this->publishStartDate = $startDate;
        }

        public function getPublishStartDate()
        {
            return $this->publishStartDate;
        }

        public function setPublishEndDate(\DateTime $endDate = null)
        {
            $this->publishEndDate = $endDate;
        }

        public function getPublishEndDate()
        {
            return $this->publishEndDate;
        }

        public function isPublishable()
        {
            return $this->isPublishable;
        }

        public function setIsPublishable($boolean)
        {
            $this->isPublishable = $boolean;
        }
    }

Elektory publikowania
~~~~~~~~~~~~~~~~~~~~~

Elektor publikowania musi implementować
:class:`Symfony\\Component\\Security\\Core\\Authorization\\Voter\\VoterInterface`.
Przekazany zostanie obiekt treści i trzeba zdecydować, czy będzie on publikowany
zgodnie z jego zasadami. Pakiet CoreBundle udostępnia kilka ogólnych eletorów
(`PublishableVoter`_ i `TimePeriodVoter`_), które sprawdzają treść pod
kątem posiadania interfejsu oferującego potrzebne metody. Jeśli treść implementuje
ten interfejs, to sprawdzany jest parametr i zwracany jest głos ``ACCESS_GRANTED``
lub ``ACCESS_DENIED``, w przeciwnym razie zwracany jest ``ACCESS_ABSTAIN``.

Głosowania mają być zgodne. Każdy elektor zwraca głos ``ACCESS_GRANTED``,
jeśli jego kryteria są spełnione, lecz jeśli jakiś elektor zwróci
``ACCESS_DENIED``, treść jest uważana za nie opublikowaną.

Można również zaimplementować :ref:`własne elektory <bundle-core-workflow-custom-voters>`
w celu uzyskania dodatkowych zachowań publikacji.

PublishableVoter
................

Elektor ten sprawdza w ``PublishableReadInterface`` czy interfejs ten ma metodę,
która zwraca wartość logiczną.

* **isPublishable**: Czy obiekt powinien być uznany za publikację.

TimePeriodVoter
...............

Elektor ten sprawdza w ``PublishTimePeriodReadInterface`` czy określona została
data początkowa i końcowa. Data o wartości null oznacza "zawsze rozpoczęte" lub
odpowiednio "nigdy się nie kończy".

* **getPublishStartDate**: Jeśli nie null, to data od której należy rozpocząć publikację;
* **getPublishEndDate**: Jeśli nie null, to data, od której należ zaprzestać publikację.

.. _bundle-core-workflow-custom-voters:

Własne elektory publikowania
............................

Do budowania elektorów z własną logiką trzeba zaimplementować
:class:`Symfony\\Component\\Security\\Core\\Authentication\\Voter\\VoterInterface`
oraz zdefiniować usługę z tagiem ``cmf_published_voter``. Podobny jest on do tagu
``security.voter``, ale dodaje indywidualny elektor do procesu publikowania.
Podobnie jak w przypadku elektorów bezpieczeństwa, można określić priorytet,
choć jest ograniczenie stosowania, gdyż decyzja o dostępie musi być jednomyślna.
Jeśli ma się więcej kosztownych procedur kontrolnych, można obniżyć priorytet tych
elektorów.

.. configuration-block::

    .. code-block:: yaml

        services:
            acme.security.publishable_voter:
                class: "%my_namespace.security.publishable_voter.class%"
                tags:
                    - { name: cmf_published_voter, priority: 30 }

    .. code-block:: xml

        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services">
            <service id="acme.security.publishable_voter"
                class="%acme.security.publishable_voter.class%">

                <tag name="cmf_published_voter" priority="30"/>
            </service>
        </container>

    .. code-block:: php

        use Symfony\Component\DependencyInjection\Definition;

        $container
            ->register(
                'acme.security.publishable_voter',
                '%acme.security.publishable_voter.class%'
            )
            ->addTag('cmf_published_voter', array('priority' => 30))
        ;

Procedura kontroli procesu publikowania tworzy w locie
:class:`Symfony\\Component\\Security\\Core\\Authentication\\Token\\AnonymousToken`,
jeśli kontekst bezpieczeństwa ma wartość none. Oznacza to, ze elektory muszą być
zdolne do obsłużenia takiej sytuacji podczas udzielania dostępu użytkownikowi.
Również podczas udzielania dostępu kontekstowi bezpieczeństwa, najpierw muszą one
sprawdzić, czy kontekst ten ma token a jeśli nie, to nie mogą kontekstu tego
wywoływać, aby uniknąć wywołania wyjątku. W przypadku, w którym elektor daje dostęp,
gdy obecny użytkownik spełnia jakieś wymagania, to musi zwrócić ``ACCESS_DENIED``,
jeśli nie ma aktualnego użytkownika.

.. _bundle-core-workflow-request_listener:

Odbiornik żądań publikacji
~~~~~~~~~~~~~~~~~~~~~~~~~~

:doc:`DynamicRouter <../routing/dynamic>` umieszcza obiekt trasy i główną treść
(jeśli trasa ma główną treść) w atrybutach żądania. Jeśli wyłączy się
``cmf_core.publish_workflow.request_listener``, to ten odbiornik będzie nasłuchiwał
wszystkie żądania i sprawdzał publikacje zarówno obiektu trasy, jak i głównego
obiektu treści.

Oznacza to, że własne szablony dla ``templates_by_class`` i kontrolery
``controllers_by_class`` nie muszą być jawnie sprawdzane w zakresie publikacji,
gdyż jest to już zrobione.

.. _bundle-core-workflow-admin-extensions:

Edytowanie informacji publikacyjnej: rozszerzenie Sonata Admin dla procesu publikowania
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Istnieje także interfejs zapisu dla każdego procesu publikowania, definiujący metody
setter. Pakiet rdzenia dostarcza rozszerzeń dla SonataAdminBundle w celu umożliwienia
łatwego dodawania edycji pól procesu publikowania dla wszystkich lub wybranych paneli
administracyjnych.

Zamiast implementować ``PublishableReadInterface``, czy też
``PublishTimePeriodReadInterface``, należy implementować ``PublishableInterface``
i odpowiednio ``PublishTimePeriodInterface``.

Dla włączenia rozszerzeń w klasach paneli administracyjnych, wystarczy zdefiniować
konfiguracje rozszerzenia w sekcji ``sonata_admin`` konfiguracji projektu:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        sonata_admin:
            # ...
            extensions:
                cmf_core.admin_extension.publish_workflow.publishable:
                    implements:
                        - Symfony\Cmf\Bundle\CoreBundle\PublishWorkflow\PublishableInterface
                cmf_core.admin_extension.publish_workflow.time_period:
                    implements:
                        - Symfony\Cmf\Bundle\CoreBundle\PublishWorkflow\PublishTimePeriodInterface

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services">
            <config xmlns="http://sonata-project.org/schema/dic/admin">
                <!-- ... -->
                <extension id="cmf_core.admin_extension.publish_workflow.publishable">
                    <implement>
                        Symfony\Cmf\Bundle\CoreBundle\PublishWorkflow\PublishableInterface
                    </implement>
                </extension>

                <extension id="cmf_core.admin_extension.publish_workflow.time_period">
                    <implement>
                        Symfony\Cmf\Bundle\CoreBundle\PublishWorkflow\PublishTimePeriodInterface
                    </implement>
                </extension>
            </config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('sonata_admin', array(
            // ...
            'extensions' => array(
                'cmf_core.admin_extension.publish_workflow.publishable' => array(
                    'implements' => array(
                        'Symfony\Cmf\Bundle\CoreBundle\PublishWorkflow\PublishableInterface',
                    ),
                ),
                'cmf_core.admin_extension.publish_workflow.time_period' => array(
                    'implements' => array(
                        'Symfony\Cmf\Bundle\CoreBundle\PublishWorkflow\PublishTimePeriodInterface',
                    ),
                ),
            ),
        ));

W celu uzyskania więcej informacji, proszę przeczytac `Dokumentację rozszerzenia Sonata Admin`_.

.. _`komponentu bezpieczeństwa Symfony2`: http://symfony.com/doc/current/components/security/index.html
.. _`Uwierzytelnianie w Symfony2`: http://symfony.com/doc/current/components/security/authorization.html
.. _`Security Chapter`: http://symfony.com/doc/current/book/security.html
.. _`procedur kontrolnych ACL`: http://symfony.com/doc/current/cookbook/security/acl.html
.. _`Dokumentację rozszerzenia Sonata Admin`: http://sonata-project.org/bundles/admin/master/doc/reference/extensions.html
