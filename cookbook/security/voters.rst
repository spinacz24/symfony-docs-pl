.. index::
   single: bezpieczeństwo; wyborcy autoryzacji
   single: autoryzacja; wyborcy

Jak używać wyborców autoryzacji do sprawdzania uprawnień użytkownika
====================================================================

W Symfony można sprawdzać uprawnienia dostępu do danych stosując
:doc:`moduł ACL </cookbook/security/acl>`, który jest dość przytłaczajacy dla
wielu aplikacji. Znacznie prostszym rozwiazaniem jest praca z własnymi "wyborcami
uwierzytelniania" (*ang. voters*), które są podobne do prostych wyrażeń warunkowych.

.. seealso::

    Wyborców uwierzytelniania można również wykorzystywać w inny sposób, taki jak
    czarna lista adresów IP dla calej aplikacji: :doc:`/cookbook/security/voters`.

.. tip::

    Proszę zapożnać się z rozdziałem
    :doc:`Autoryzacja </components/security/authorization>`,
    który pomaga w zrozumieniu wyborców autoryzacji.

Jak Symfony używa wyborców autoryzacji
--------------------------------------

W celu używania wyborców autoryzacji, trzeba zrozumieć to, jak Symfony z nimi
pracuje.
Wszyscy wyborcy autoryzacji są wywoływani za każdym razem, jak użyta zostaje
metoda ``isGranted()`` w usłudze Symfony sprawdzającej autoryzację (czyli
``security.authorization_checker``). Każdy z wyborców decyduje, czy bieżący użytkownik
powinien mieć dostęp do pewnych zasobów.

Ostatecznie, Symfony pobiera odpowiedzi od wszystkich wyborców i decyduje ostatecznie
o umożliwieniu lub zabronieniu dostępu do zasobu. Odbywa sie to zgodnie że zdefiniowaną
w aplikacji strategią, która może być: pozytywna (*ang. affirmative*),
większościowa (*ang.  consensus*) lub jednomyślna (*ang. unanimous*).

Wiecej informacji można uzyskać w
:ref:`rozdziale poświeconym menadżerom decyzji dostępu <components-security-access-decision-manager>`.

Intefejs Voter
--------------

We własnej klasie wyborcy autoryzacji trzeba zaimplementowac interfejs
:class:`Symfony\\Component\\Security\\Core\\Authorization\\Voter\\VoterInterface`
lub rozszerzyć klasę :class:`Symfony\\Component\\Security\\Core\\Authorization\\Voter\\AbstractVoter`,
co znacznie ułatwia tworzenie klasy wyborcy.

.. code-block:: php
   :linenos:

    abstract class AbstractVoter implements VoterInterface
    {
        abstract protected function getSupportedClasses();
        abstract protected function getSupportedAttributes();
        abstract protected function isGranted($attribute, $object, $user = null);
    }

W naszym przykładzie, wyborcy będą sprawdzać, czy użytkownik ma dostęp do
określonego obiektu, zgodnie z własnymi warunkami (np. muszą być one właścicielem
obiektu). Jeśli warunek nie będzie spełniony, klasa zwracać będzie
``VoterInterface::ACCESS_DENIED`` a w przeciwnym przypadku ``VoterInterface::ACCESS_GRANTED``.
W przypadku, gdy odpowiedzialność za decyzję nie należy do wyborcy, zwróci on
``VoterInterface::ACCESS_ABSTAIN``.

Tworzenie własnych wyborców autoryzacji
---------------------------------------

Naszym celem jest stworzenie wyborcy autoryzacji, który sparwdza, czy użytkownik
ma dostęp do widoku lub edycji określonego obiektu. Oto przykład implementacji:

.. code-block:: php
   :linenos:

    // src/AppBundle/Security/Authorization/Voter/PostVoter.php
    namespace AppBundle\Security\Authorization\Voter;

    use Symfony\Component\Security\Core\Authorization\Voter\AbstractVoter;
    use AppBundle\Entity\User;
    use Symfony\Component\Security\Core\User\UserInterface;

    class PostVoter extends AbstractVoter
    {
        const VIEW = 'view';
        const EDIT = 'edit';

        protected function getSupportedAttributes()
        {
            return array(self::VIEW, self::EDIT);
        }

        protected function getSupportedClasses()
        {
            return array('AppBundle\Entity\Post');
        }

        protected function isGranted($attribute, $post, $user = null)
        {
            // make sure there is a user object (i.e. that the user is logged in)
            if (!$user instanceof UserInterface) {
                return false;
            }

            // double-check that the User object is the expected entity.
            // It always will be, unless there is some misconfiguration of the
            // security system.
            if (!$user instanceof User) {
                throw new \LogicException('The user is somehow not our User class!');
            }

            switch($attribute) {
                case self::VIEW:
                    // the data object could have for example a method isPrivate()
                    // which checks the Boolean attribute $private
                    if (!$post->isPrivate()) {
                        return true;
                    }

                    break;
                case self::EDIT:
                    // this assumes that the data object has a getOwner() method
                    // to get the entity of the user who owns this data object
                    if ($user->getId() === $post->getOwner()->getId()) {
                        return true;
                    }

                    break;
            }
            
            return false;
        }
    }

To jest to! Wyborca autoryzacji został wykonany. Następnym krokiem jest wstrzyknięcie
wyborcy do warstwy bezpieczeństwa.

Dla przypomnienia, oto co możemy oczekiwać od trzech abstarkcyjnych metod:

:method:`Symfony\\Component\\Security\\Core\\Authorization\\Voter\\AbstractVoter::getSupportedClasses`
    Informuje Symfony, że wyborca powinien zostać wywołany, gdy obiekt jednej
    z podanych klas został przekazany do metody ``isGranted()``. Na przykład,
    jeśli zwrócona zostanie tablica ``array('AppBundle\Model\Product')``, Symfony
    wywoła wyborcę, gdy obiekt ``Product`` zostanie przekazany do ``isGranted()``.

:method:`Symfony\\Component\\Security\\Core\\Authorization\\Voter\\AbstractVoter::getSupportedAttributes`
    Powiadamia Symfony, że wyborca powinien zostać wywołany, gdy jeden z podanych
    łańcuchów tekststowych został przekazany jako pierwszy argument metody ``isGranted()``.
    Na przykład, jeśli zwracana jest tablica ``array('CREATE', 'READ')``, Symfony
    wywoła wyborcę, gdy jeden z elementów tej tablicy został przekazany do metody
    ``isGranted()``.

:method:`Symfony\\Component\\Security\\Core\\Authorization\\Voter\\AbstractVoter::isGranted`
    Implementuje logikę biznesową, która sprawdza, czy dany użytkownik jest uprawniony
    do dostępu do podanego atrybutu (np. ``CREATE`` lub ``READ``) w danym obiekcie.
    Metoda ta musi zwracać wartość logiczną.

.. note::

    Obecnie, aby używać bazową klasę ``AbstractVoter``, trzeba utworzyć klasę
    wyborcy, której obiekt jest zawsze przekazywany do metody ``isGranted()``.

Deklarowanie wyborcy autoryzacji jako usługi
--------------------------------------------

Do wstrzykniecia klasy wyborcy do wartswy bezpieczeństwa trzeba zadeklarować ją
jako usługę i oflagować tagiem ``security.voter``:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/services.yml
        services:
            security.access.post_voter:
                class:      AppBundle\Security\Authorization\Voter\PostVoter
                public:     false
                tags:
                    - { name: security.voter }

    .. code-block:: xml
       :linenos:

        <!-- app/config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="security.access.post_voter"
                    class="AppBundle\Security\Authorization\Voter\PostVoter"
                    public="false">

                    <tag name="security.voter" />
                </service>
            </services>
        </container>

    .. code-block:: php
       :linenos:

        // app/config/services.php
        use Symfony\Component\DependencyInjection\Definition;
        
        $definition = new Definition('AppBundle\Security\Authorization\Voter\PostVoter');
        $definition
            ->setPublic(false)
            ->addTag('security.voter')
        ;

        $container->setDefinition('security.access.post_voter', $definition);

Jak stosować wyborców autoryzacji w kontrolerze
-----------------------------------------------

Zarejestrowana usługa wyborcy jest odpytywana zawsze, gdy wywoływana zostaje metoda
``isGranted()`` z usługi sprawdzania autoryzacji ``security.authorization_checker``
Symfony.

.. code-block:: php
   :linenos:

    // src/AppBundle/Controller/PostController.php
    namespace AppBundle\Controller;

    use Symfony\Bundle\FrameworkBundle\Controller\Controller;
    use Symfony\Component\HttpFoundation\Response;

    class PostController extends Controller
    {
        public function showAction($id)
        {
            // get a Post instance
            $post = ...;

            $authChecker = $this->get('security.authorization_checker');

            $this->denyAccessUnlessGranted('view', $post, 'Unauthorized access!');

            return new Response('<h1>'.$post->getName().'</h1>');
        }
    }

.. versionadded:: 2.6
    Usługa ``security.authorization_checker`` została wprowadzona w Symfony 2.6.
    Wcześniej trzeba było stosować metodę ``isGranted()`` usługi
    ``security.context``.

To takie proste, prawda?

.. index::
   single: autoryzacja; strategie decyzyjne

.. _security-voters-change-strategy:

Zmienianie strategii decyzyjnej dostępu
---------------------------------------

Proszę sobie wyobrazić, że mamy wielu wyborców autoryzacji dla jednej akcji obiektu.
Na przykład, mamy jednego wyborcę, który sprawdza, czy użytkownik jest członkiem
witryny oraz drugiego wyborcę, który sprawdza, czy użytkownik skończył 18 lat.

Dla rozwiązania takich przypadków, menadżer decyzji dostępu używa strategii decyzyjnej
dostępu. Można je dostosować do własnych potrzeb. Dostępne są trzy strategie:

``affirmative`` (pozytywna, będąca strategią domyślną)
    Udzielenie dostępu, gdy chociaż jeden wyborca głosuje za przydzieleniem dostępu;

``consensus`` (większościowa)
    Udzielenie dostępu, gdy większość wyborców głosuje za jego przydzieleniem;

``unanimous`` (jednomyślna)
    Udzielenie dostępu, gdy wszyscy wyborcy głosuja za przydzieleniem dostępu.

W przyjętym scenariuszu obydwaj wyborcy powinny przydzielać dostęp, aby użytkownik
uzyskał dostęp do odczytu wpisu. W takim przypadku, domyślna strategia nie jest
dalej ważna i zamiast niej powinna zostać użyta strategia ``unanimous``.
Można to ustawić w konfiguracji bezpieczeństwa:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            access_decision_manager:
                strategy: unanimous

    .. code-block:: xml
       :linenos:

        <!-- app/config/security.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd"
        >

            <config>
                <access-decision-manager strategy="unanimous">
            </config>
        </srv:container>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'access_decision_manager' => array(
                'strategy' => 'unanimous',
            ),
        ));
