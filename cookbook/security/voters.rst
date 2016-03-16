.. highlight:: php
   :linenothreshold: 2

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

    Proszę zapoznać się z rozdziałem
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
lub rozszerzyć klasę :class:`Symfony\\Component\\Security\\Core\\Authorization\\Voter\\Voter`,
co znacznie ułatwia tworzenie klasy wyborcy.

.. code-block:: php
   :linenos:
   
   abstract class Voter implements VoterInterface
    {
        abstract protected function supports($attribute, $subject);
        abstract protected function voteOnAttribute($attribute, $subject, TokenInterface $token);
    }

.. versionadded:: 2.8
    Klasa pomocnicza ``Voter`` została wprowadzona w Symfony 2.8. we wcześniejszych
    wersjach stosowana była klasa ``AbstractVoter`` o podobnym zachowaniu.    

.. _how-to-use-the-voter-in-a-controller:

Konfiguracja: Sprawdzanie dostępu do kontrolera
-----------------------------------------------

Załóżmy, że mamy obiekt ``Post`` i potrzebujemy zadecydować, czy bieżący użytkownik
może edytować lub przeglądać obiekt. W kontrolerze można sprawdzić dostęp następujaco::

    // src/AppBundle/Controller/PostController.php
    // ...

    class PostController extends Controller
    {
        /**
         * @Route("/posts/{id}", name="post_show")
         */
        public function showAction($id)
        {
            // get a Post object - e.g. query for it
            $post = ...;

            // check for "view" access: calls all voters
            $this->denyAccessUnlessGranted('view', $post);

            // ...
        }

        /**
         * @Route("/posts/{id}/edit", name="post_edit")
         */
        public function editAction($id)
        {
            // get a Post object - e.g. query for it
            $post = ...;

            // check for "edit" access: calls all voters
            $this->denyAccessUnlessGranted('edit', $post);

            // ...
        }
    }

Metoda ``denyAccessUnlessGranted()`` (jak równiez prostsza metoda ``isGranted()``)
wywołuje mechanizm "wyborcy autoryzacji" (*ang. voter*). W tej chwili żaden
wyborca autoryzacji nie głosuje, czy użytkownik może uruchamiać metodę przegladania
lub edytowania obiektu ``Post``. Lecz można utworzyć własnego wyborcę
autoryzacji, który będzie o tym decydował.

.. tip::

    Funkcje ``denyAccessUnlessGranted()`` i ``isGranted()`` są obie skrótem
    do wywoływania ``isGranted()`` w usłudzue ``security.authorization_checker``.
    

Tworzenie własnych wyborców autoryzacji
---------------------------------------

Przyjmijmy, że mamy logikę do decydowania, czy użytkownik może przeglądać lub edytować
obiekt ``Post``, która jest dość skomplikowana. Na przykład, ``User`` może zawsze
edytować lub przeglądać obiekty ``Post``, których jest autorem. Jeśli obiekt
``Post`` jest oznaczony jako "public", przeglądać go mogą wszyscy. W tej sytuacji,
wyborca autoryzacji bedzie wyglądał podobnie do tego::

    // src/AppBundle/Security/PostVoter.php
    namespace AppBundle\Security;

    use AppBundle\Entity\Post;
    use AppBundle\Entity\User;
    use Symfony\Component\Security\Core\Authentication\Token\TokenInterface;
    use Symfony\Component\Security\Core\Authorization\Voter\Voter;

    class PostVoter extends Voter
    {
        // these strings are just invented: you can use anything
        const VIEW = 'view';
        const EDIT = 'edit';

        protected function supports($attribute, $subject)
        {
            // jeśli $attributw nie jest tym, co obsługujemy, zwracane jest false
            if (!in_array($attribute, array(self::VIEW, self::EDIT))) {
                return false;
            }

            // głosowanie tylko na obiekt Post
            if (!$subject instanceof Post) {
                return false;
            }

            return true;
        }

        protected function voteOnAttribute($attribute, $subject, TokenInterface $token)
        {
            $user = $token->getUser();

            if (!$user instanceof User) {
                // the user must be logged in; if not, deny access
                return false;
            }

            // you know $subject is a Post object, thanks to supports
            /** @var Post $post */
            $post = $subject;

            switch($attribute) {
                case self::VIEW:
                    return $this->canView($post, $user);
                case self::EDIT:
                    return $this->canEdit($post, $user);
            }

            throw new \LogicException('This code should not be reached!');
        }

        private function canView(Post $post, User $user)
        {
            // if they can edit, they can view
            if ($this->canEdit($post, $user)) {
                return true;
            }

            // the Post object could have, for example, a method isPrivate()
            // that checks a boolean $private property
            return !$post->isPrivate();
        }

        private function canEdit(Post $post, User $user)
        {
            // this assumes that the data object has a getOwner() method
            // to get the entity of the user who owns this data object
            return $user === $post->getOwner();
        }
    }

To jest to! Wyborca autoryzacji został wykonany. Następnie musimy go
:ref:`skonfigurować <declaring-the-voter-as-a-service>`

Dla przypomnienia, oto co możemy oczekiwać od tych dwóch abstrakcyjnych metod:

``Voter::supports($attribute, $subject)``
    Gdy wywoływana jest metoda ``isGranted()`` (lub ``denyAccessUnlessGranted()``),
    jako pierwszy argument jest przekazywana tutaj zmienna ``$attribute`` (np.
    ``ROLE_USER``, ``edit``) a jako drugi argument (jeśli istnieje) zmienna
    ``$subject`` (np. ``null``, obiekt ``Post``). Zadaniem tych argumentów jest
    określenie, jak wyborca powinien "głosować". Jeśli zwrócimy
    ``true``, wywoływana będzie metoda ``voteOnAttribute()``. W przeciwnym razie,
    wyborca nie bedzie głosował: niektóre inne usługi wyborcze to jeszcze przetwarzają.
    W naszym przykładzie, zwracamy ``true``, jeśli ``$attribut`` ma wartość ``view``
    lub ``edit`` i jeśli obiekt jest instancją ``Post``.

``voteOnAttribute($attribute, $subject, TokenInterface $token)``
    Metoda ta zostaje wywołana, jeśli metoda ``supports()`` zwraca ``true``.
    Nasze zadanie jest proste: zwrócić ``true``, aby zezwolić na dostęp a ``false``,
    aby zabronić dostępu.
    Argument ``$token`` można wykorzystać do znalezienia bieżącego obiektu użytkownika
    (jeśli istnieje). W naszym przykładzie, cała złożonona logika biznesowa została
    dołączona w celu określenia dostępu.

.. _declaring-the-voter-as-a-service:

Konfigurowanie wyborcy autoryzacji
----------------------------------

Do wstrzykniecia klasy wyborcy do wartswy bezpieczeństwa trzeba zadeklarować ją
jako usługę i oflagować tagiem ``security.voter``:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/services.yml
        services:
            app.post_voter:
                class: AppBundle\Security\PostVoter
                tags:
                    - { name: security.voter }
                # small performance boost
                public: false

    .. code-block:: xml
       :linenos:

        <!-- app/config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="app.post_voter"
                    class="AppBundle\Security\PostVoter"
                    public="false"
                >

                    <tag name="security.voter" />
                </service>
            </services>
        </container>

    .. code-block:: php
       :linenos:

        // app/config/services.php
        use Symfony\Component\DependencyInjection\Definition;

        $container->register('app.post_voter', 'AppBundle\Security\PostVoter')
            ->setPublic(false)
            ->addTag('security.voter')
        ;

Teraz, gdy wywoła się :ref:`isGranted() z view/edit i obiektem Post object <how-to-use-the-voter-in-a-controller>`,
wykonany zostanie wyborca autoryzacji i będzie można autoryzować użytkownika.

.. index::
   single: autoryzacja; role wewnątrz wyborców 

Sprawdzanie ról wewnątrz wyborców
---------------------------------

.. versionadded:: 2.8
    W Symfony 2.8 wprowadzono możliwość wstrzykiwania ``AccessDecisionManager``,
    co powodowało wcześniej zrzucenie wyjątku CircularReferenceException.
    We wcześniejszych wersjach, dla możliwości użycia ``isGranted()``, trzeba było
    wstrzykiwać sam ``service_container`` i wyprowadzać ``security.authorization_checker``.

Co zrobić, jeśli chce się wywołać ``isGranted()`` z wnętrza wyborcy autoryzacji,
np. jeśli chce się  zobaczyć, czy bieżący użytkownik ma rolę ``ROLE_SUPER_ADMIN``.
Jest to możliwe przez wstrzyknięcie
:class:`Symfony\\Component\\Security\\Core\\Authorization\\AccessDecisionManager`
do klasy wyborcy. Można to używać, na przykład, aby *zawsze* umożliwiać dostęp
dla użytkownika z rolą ``ROLE_SUPER_ADMIN``::

    // src/AppBundle/Security/PostVoter.php

    // ...
    use Symfony\Component\Security\Core\Authorization\AccessDecisionManagerInterface;

    class PostVoter extends Voter
    {
        // ...

        private $decisionManager;

        public function __construct(AccessDecisionManagerInterface $decisionManager)
        {
            $this->decisionManager = $decisionManager;
        }

        protected function voteOnAttribute($attribute, $subject, TokenInterface $token)
        {
            // ...

            // ROLE_SUPER_ADMIN can do anything! The power!
            if ($this->decisionManager->decide($token, array('ROLE_SUPER_ADMIN'))) {
                return true;
            }

            // ... all the normal voter logic
        }
    }

Następnie, trzeba zaktualizować ``services.yml``, wstrzykując usługę
``security.access.decision_manager``:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/services.yml
        services:
            app.post_voter:
                class: AppBundle\Security\PostVoter
                arguments: ['@security.access.decision_manager']
                public: false
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
                <service id="app.post_voter"
                    class="AppBundle\Security\PostVoter"
                    public="false"
                >
                    <argument type="service" id="security.access.decision_manager"/>

                    <tag name="security.voter" />
                </service>
            </services>
        </container>

    .. code-block:: php
       :linenos:

        // app/config/services.php
        use Symfony\Component\DependencyInjection\Definition;
        use Symfony\Component\DependencyInjection\Reference;

        $container->register('app.post_voter', 'AppBundle\Security\PostVoter')
            ->addArgument(new Reference('security.access.decision_manager'))
            ->setPublic(false)
            ->addTag('security.voter')
        ;

To wszystko! Wywołanie ``decide()`` na ``AccessDecisionManager`` jest w zasadzie
tym samym, co wywołanie ``isGranted()`` z poziomu kontrolera lub innego miejsca 
(to jest tylko nieco niższy poziom, co jest niezbędne dla wyborców).

.. note::

    Usługa ``security.access.decision_manager`` jest prywatna. Oznacza to, że
    nie mozna jest wywołać bezpośrednio z poziomu kontrolera - można ja tylko
    wstrzyknąć do innych usług. Jest właściwe, aby zamiast tego stosować
    ``security.authorization_checker`` we wszystkich przypadkach, z wyjątkiem
    wyborców autoryzacji.

.. index::
   single: autoryzacja; strategie decyzyjne

.. _security-voters-change-strategy:

Zmienianie strategii decyzyjnej dostępu
---------------------------------------

Zwykle, w jednym momencie "głosuje" tylko jeden wyborca (reszta będzie się "wstrzymywać",
co oznacza, że będa one zwracać ``false`` w metodzie ``supports()``).
Lecz teoretycznie, mozna przeprowadzić "głosowanie" wielu wyborców dla jednej akcji
lub obiektu. Dla przykładu przyjmijmy, że mamy jednego wyborcę, ktry sprawdza, czy
uzytkownik jest członkiem witryny i drugiego, który sprawdza czy uzytkownik skończył
18 lat.

Dla rozwiązania takich przypadków, menadżer decyzji dostępu
(``security.access.decision_manager``) używa strategii decyzyjnej
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
