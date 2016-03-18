.. highlight:: php
   :linenothreshold: 2

.. index::
   single: bezpieczeństwo; autoryzacja
   double: komponent Security; autoryzacja

Autoryzacja
===========

Gdy jakiś wystawca uwierzytelniania (patrz :ref:`authentication_providers`)
zweryfikował jeszcze nie uwierzytelniony token, zostanie zwrócony token
uwierzytelniony. Detektor uwierzytelniania powinien ustawiać ten token bezpośrednio
w magazynie tokenów (obiekcie klasy
:class:`Symfony\\Component\\Security\\Core\\Authentication\\Token\\Storage\\TokenStorageInterface`)
przy użyciu metody
 :method:`Symfony\\Component\\Security\\Core\\Authentication\\Token\\Storage\\TokenStorageInterface::setToken`.

Od tego momentu użytkownik zostaje uwierzytelniony, czyli zidentyfikowany. Teraz,
różne części aplikacji mogą wykorzystywać ten token do decydowania, czy użytkownik
może żądać  określonego zasobu URI lub modyfikować określony obiekt. Decyzje te
są realizowane przez instancję klasy
:class:`Symfony\\Component\\Security\\Core\\Authorization\\AccessDecisionManagerInterface`.

Decyzja autoryzacyjna zawsze opiera się na kilku rzeczach:

* bieżącym tokenie
    Na przykład, do pobrania ról dla bieżącego użytkownika może być użyta metoda
    :method:`Symfony\\Component\\Security\\Core\\Authentication\\Token\\TokenInterface::getRoles`
    tokenu  
    (np. ``ROLE_SUPER_ADMIN``) lub decyzja może zostać oparta na klasie tego tokenu.
* zestawie atrybutów
    Każdy atrybut wskazuje na określone uprawnienie, jakie powinien mieć użytkownik,
    np. ``ROLE_ADMIN`` dla administratora.
* obiekcie (opcjonalnie)
    Dowolny obiekt dla którego trzeba sprawdzić prawo dostępu, taki jak obiekt
    artykułu lub komentarza.

.. index::
   single: autoryzacja; menadżer decyzji dostępu

.. _components-security-access-decision-manager:

Menadżer dezycji dostępu
------------------------

Ponieważ o autoryzacji użytkownika może być skomplikowanym procesem, standardowa
klasa :class:`Symfony\\Component\\Security\\Core\\Authorization\\AccessDecisionManager`
sama zależy od wielu wyborców autoryzacji i sprawia, że ostateczny werdykt jest
oparty na wielu głosach (zarówno pozytywnych, negatywnych jak i neutralnych),
które otrzymał. Rozpoznawanych jest kilka strategii:

``affirmative`` (domyślna)
    dostęp przyznawany jest, jeśli chociaż tylko jeden z wyborców autoryzacji
    taki dostęp przyznaje;

``consensus``
    dostęp przyznawany jest, jeśli większość wyborców go przyznaje;

``unanimous``
    dostęp przyznawany jest, jeśli żaden z wyboróców nie zabrania dostępu.

.. code-block:: php
   :linenos:

    use Symfony\Component\Security\Core\Authorization\AccessDecisionManager;

    // instancje Symfony\Component\Security\Core\Authorization\Voter\VoterInterface
    $voters = array(...);

    // jedno z "affirmative", "consensus", "unanimous"
    $strategy = ...;

    // czy należy udzielić dostępu, gdy wszyscy wyborcy wstrzymaja sie od głosu
    $allowIfAllAbstainDecisions = ...;

    // czy należy udzielić dostępu, gdy nie ma większości (odnosi się tylko do strategii "consensus" )
    $allowIfEqualGrantedDeniedDecisions = ...;

    $accessDecisionManager = new AccessDecisionManager(
        $voters,
        $strategy,
        $allowIfAllAbstainDecisions,
        $allowIfEqualGrantedDeniedDecisions
    );

.. seealso::

    Można zmienić domyślną strategię w :ref:`konfiguracji <security-voters-change-strategy>`.

.. index::
   single: autoryzacja; wyborcy

Wyborcy autoryzacji
-------------------

Wyborca autoryzacji jest obiektem będącym instancją
:class:`Symfony\\Component\\Security\\Core\\Authorization\\Voter\\VoterInterface`,
co oznacza, że trzeba w takiej klasie zaimplementować kilka metod, umożliwiających
podejmowanie decyzji przez menadżera decyzji:

``supportsAttribute($attribute)`` (zdeprecjonowany w 2.8)
    używana jest do sprawdzenia, czy wyborca wie, jak obsługiwać określony atrybut;

``supportsClass($class)`` (zdeprecjonowany w 2.8)
    stosowana jest do określenia, czy wyborca może przyznawać lub odmawiać dostęp
    do obiektu określonej klasy;

``vote(TokenInterface $token, $object, array $attributes)``
    realizuje faktyczne głosowanie i zwraca wartość równą jednej ze stałych
    określonych w :class:`Symfony\\Component\\Security\\Core\\Authorization\\Voter\\VoterInterface`,
    czyli ``VoterInterface::ACCESS_GRANTED``, ``VoterInterface::ACCESS_DENIED``
    lub ``VoterInterface::ACCESS_ABSTAIN``;

.. note::

    Metody ``supportsAttribute()`` i ``supportsClass()`` zostały zdeprecjonowane
    w Symfony 2.8 i nie będą obsługiwane w Symfony 3.0. Metody te nie powinny
    być wywoływane na zewnątrz klasy wyborcy.

Komponent Security zawiera kilka standardowych wyborców autoryzacji, które mają
zastosowanie w większości typowych przypadków:

AuthenticatedVoter
~~~~~~~~~~~~~~~~~~

Wyborca :class:`Symfony\\Component\\Security\\Core\\Authorization\\Voter\\AuthenticatedVoter`
obsługuje atrybuty ``IS_AUTHENTICATED_FULLY``, ``IS_AUTHENTICATED_REMEMBERED``
i ``IS_AUTHENTICATED_ANONYMOUSLY`` i przyznaje dostęp na podstawie aktualnego
poziomu uwierzytelniania, czyli czy użtkownik został w pełni uwierzytelniony, czy
tylko na podstawie ciasteczka "remember-me" czy też został uwierzytelniony anonimowo.

.. code-block:: php
   :linenos:

    use Symfony\Component\Security\Core\Authentication\AuthenticationTrustResolver;

    $anonymousClass = 'Symfony\Component\Security\Core\Authentication\Token\AnonymousToken';
    $rememberMeClass = 'Symfony\Component\Security\Core\Authentication\Token\RememberMeToken';

    $trustResolver = new AuthenticationTrustResolver($anonymousClass, $rememberMeClass);

    $authenticatedVoter = new AuthenticatedVoter($trustResolver);

    // instance of Symfony\Component\Security\Core\Authentication\Token\TokenInterface
    $token = ...;

    // any object
    $object = ...;

    $vote = $authenticatedVoter->vote($token, $object, array('IS_AUTHENTICATED_FULLY');

RoleVoter
~~~~~~~~~

Wyborca :class:`Symfony\\Component\\Security\\Core\\Authorization\\Voter\\RoleVoter`
obsługuje atrybuty rozpoczynające się od ``ROLE_`` i przydziela dostęp dla użytkownika,
gdy wymagane atrybuty ``ROLE_*`` mogą być odnalezione w tablicy ról zwracanej przez
metodę :method:`Symfony\\Component\\Security\\Core\\Authentication\\Token\\TokenInterface::getRoles`
tokenu::

    use Symfony\Component\Security\Core\Authorization\Voter\RoleVoter;

    $roleVoter = new RoleVoter('ROLE_');

    $roleVoter->vote($token, $object, array('ROLE_ADMIN'));

RoleHierarchyVoter
~~~~~~~~~~~~~~~~~~

Wyborca :class:`Symfony\\Component\\Security\\Core\\Authorization\\Voter\\RoleHierarchyVoter`
rozszerza :class:`Symfony\\Component\\Security\\Core\\Authorization\\Voter\\RoleVoter`
i dostarcza kilka dodatkowych funkcjonalności: wie jak obsługiwać hierarchię
ról. Na przykład, rola ``ROLE_SUPER_ADMIN`` może mieć podrole
``ROLE_ADMIN`` i ``ROLE_USER``, tak więc gdy jakiś obiekt wymaga od użytkownika
posiadania roli ``ROLE_ADMIN``, to przyznawany jest dostęp użytkownikom, którzy
faktycznie maja rolę ``ROLE_ADMIN``, ale też tym z rola ``ROLE_SUPER_ADMIN``::

    use Symfony\Component\Security\Core\Authorization\Voter\RoleHierarchyVoter;
    use Symfony\Component\Security\Core\Role\RoleHierarchy;

    $hierarchy = array(
        'ROLE_SUPER_ADMIN' => array('ROLE_ADMIN', 'ROLE_USER'),
    );

    $roleHierarchy = new RoleHierarchy($hierarchy);

    $roleHierarchyVoter = new RoleHierarchyVoter($roleHierarchy);

.. note::

    Kiedy wykonuje sie wlasna klasę wyborcy, można oczywiście wykorzystać
    jej konstruktor do wstrzyknięcia jakichkolwiek zależności, jakie są potrzebne
    do podjecia decyzji.

Role
----

Role są obiektami, które nadaja jakieś znaczenie prawom dostępu posiadanym przez
użytkownika.
Jedynym wymaganiem jest to, aby klasa roli implementowała interfejs
:class:`Symfony\\Component\\Security\\Core\\Role\\RoleInterface`,
co oznacza, że powinna mieć również metodę
:method:`Symfony\\Component\\Security\\Core\\Role\\RoleInterface::getRole`
zwracającą łańcuch reprezentujacy samą rolę::

    use Symfony\Component\Security\Core\Role\Role;

    $role = new Role('ROLE_ADMIN');

    // will show 'ROLE_ADMIN'
    var_dump($role->getRole());

.. note::

    Większość tokenów uwierzytelniania rozszerza klasę
    :class:`Symfony\\Component\\Security\\Core\\Authentication\\Token\\AbstractToken`,
    co oznacza że role podane do konstruktora tej klasy będą automatycznie przekształcane
    z łańcucha tekstowego do prostych obiektów ``Role``.

Stosowanie menadżera decyzji
----------------------------

.. index::
   simple: autoryzacja; detektor dostępu

Detektor dostępu
~~~~~~~~~~~~~~~~

Menadżer decyzji dostępu może być stosować w każdym momencie przetwarzania żądania,
aby zdecydować, czy bieżący użytkownik jest uprawniony do dostępu do określonego
zasobu. Opcjonalnym, ale użytecznym sposobem ograniczającym dostęp na podstawie
wzorca URL jest detektor dostępu
:class:`Symfony\\Component\\Security\\Http\\Firewall\\AccessListener`,
który jest jednym z detektorów zapory (patrz :ref:`firewall_listeners`), 
wyzwalanym dla każdego żądania zgodnego z mapą zapory (patrz :ref:`firewall`).

Mechanizm ten wykorzytuje mapę dostępu (która powinna być instancją
:class:`Symfony\\Component\\Security\\Http\\AccessMapInterface`),
zawierającą kod dopasowujący żądanie i odpowiedni zestaw atrybutów, jakie są
wymagane dla bieżącego użytkownika, aby uzyskać dostęp do aplikacji::

    use Symfony\Component\Security\Http\AccessMap;
    use Symfony\Component\HttpFoundation\RequestMatcher;
    use Symfony\Component\Security\Http\Firewall\AccessListener;

    $accessMap = new AccessMap();
    $requestMatcher = new RequestMatcher('^/admin');
    $accessMap->add($requestMatcher, array('ROLE_ADMIN'));

    $accessListener = new AccessListener(
        $securityContext,
        $accessDecisionManager,
        $accessMap,
        $authenticationManager
    );

.. index::
   single: autoryzacja; sprawdzanie autoryzacji

Mechanizm sprawdzający autoryzację
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Menadżer decyzji dostępu jest również dostępny dla innych części aplikacji
poprzez metodę
:method:`Symfony\\Component\\Security\\Core\\Authorization\\AuthorizationChecker::isGranted`.
Wywolanie tej metody spowoduje bezpośrednie delegowanie zapytania do menadżera
decyzji dostępu::

    use Symfony\Component\Security\Core\Authorization\AuthorizationChecker;
    use Symfony\Component\Security\Core\Exception\AccessDeniedException;

    $authorizationChecker = new AuthorizationChecker(
        $tokenStorage,
        $authenticationManager,
        $accessDecisionManager
    );

    if (!$authorizationChecker->isGranted('ROLE_ADMIN')) {
        throw new AccessDeniedException();
    }

