.. index::
   double: najlepsze praktyki; bezpieczeństwo
   

Bezpieczeństwo
==============

Uwierzytelnianie i zapory (czyli uzyskiwanie poświadczeń użytkownika)
---------------------------------------------------------------------

Można skonfigurować Symfony do uwierzytelniania użytkowników różnymi metodami
oraz ładowania informacji o użytkownikach z różnych źródeł. Jest to skomplikowany
temat, ale rozdział :doc:`"Bezpieczeństwo"</cookbook/security/index>` w Receptariuszu
ma wiele informacji z tego zakresu.

Uwierzytelnianie jest skonfigurowane w pliku ``security.yml``,
przede wszystkim w kluczu ``firewalls``.

.. best-practice::

    Dopóki nie ma się dwóch różnych systemów uwierzytelniania i dwóch różnych
    zestawów użytkowników (np. logowanie formularzowe na stronie głównej a
    system tokenowy tylko dla swojego API), zaleca się posiadanie tylko *jednej*
    skonfigurowanej zapory z włączonym kluczem ``anonymous``.

Większość aplikacji ma tylko jeden system uwierzytelniania i tylko jeden zestaw
użytkowników.
Dlatego też wystarczy mieć tylko *jedną* skonfigurowaną zaporę. Oczywiście są
wyjątki, zwłaszcza jeśli ma się oddzielne sekcje web i API na swojej witrynie.
Jednak chodzi o to, aby zbyt nie komplikować sobie życia.

Dodatkowo, należy w ramach zapory użyć klucza ``anonymous``. Jeśli jest potrzeba
wymagania od użytkowników logowania się w różnych sekcjach witryny (albo prawie
we *wszystkich* sekcjach), trzeba skonfigurować klucz ``access_control``.

.. best-practice::

    Wykorzystuj szyfrowanie ``bcrypt`` do kodowania haseł użytkowników.

Zalecamy kodowanie haseł użytkownika przy użyciu kodera ``bcrypt``,
zamiast tradycyjnego kodera mieszającego SHA-512. Główne zalety ``bcrypt``, to
włączenie wartości *soli* w celu ochrony przed wachlarzem ataków tabelowych i jego
adaptacyjny charakter, czyniący go odporniejszym na ataki siłowego wyszukiwania hasła.

Mając to na uwadze, prezentujemy poniżej konfigurację uwierzytelniania dla aplikacji,
która wykorzystuje logowanie formularzowe do załadowania użytkowników z bazy danych:

.. code-block:: yaml
   :linenos:

    # app/config/security.yml
    security:
        encoders:
            AppBundle\Entity\User: bcrypt

        providers:
            database_users:
                entity: { class: AppBundle:User, property: username }

        firewalls:
            secured_area:
                pattern: ^/
                anonymous: true
                form_login:
                    check_path: security_login_check
                    login_path: security_login_form

                logout:
                    path: security_logout
                    target: homepage

    # ... sekcja access_control istnieje, ale nie pokazano jej tutaj

.. tip::

    Kod źródłowy naszego projektu zawiera komentarze, które wyjaśniają każdą
    część.

Autoryzacja (czyli odmowa dostępu)
----------------------------------

Symfony dostarcza kilka sposobów na wymuszanie autoryzacji, w tym konfigurację
``access_control`` w :doc:`security.yml </reference/configuration/security>`,
:ref:`adnotację @Security <best-practices-security-annotation>` oraz użycie
:ref:`isGranted <best-practices-directly-isGranted>` bezpośrednio na usłudze
``security.authorization_checker``.

.. best-practice::

    * Do ochrony ogólnych wzorców URL wykorzystuj ``access_control``;
    * Jeśli to możliwe, używaj adnotacji ``@Security``;
    * Jeśli masz bardziej złożoną sytuację, bezpośrednio autoryzuj użytkownika 
      wykorzystujac usługę ``security.authorization_checker``.

Istnieją również różne sposoby scentralizowanie logiki autoryzacyjnej, takie jak
indywidualni wyborcy autoryzacji lub listy ACL.

.. best-practice::

    * Dla ustawienia bardziej szczegółowych ograniczeń, zdefinuj własnego wyborcę
      autoryzacji;
    * W celu ograniczenia dostępu do *każdego* obiektu przez *jakiegokolwiek*
      użytkownika poprzez interfejs administracyjny, użyj list ACL Symfony.

.. index::
   single: adnontacje; @Security 

.. _best-practices-security-annotation:

Adnotacja @Security
-------------------

Do kontroli dostępu na bazie akcji kontrolera, użyj w miarę możliwości adnotacji
``@Security``. Jest ona łatwa do odczytania i umieszczana konsekwentnie nad
zabezpieczaną akcją.

W naszej aplikacji, do tworzenia nowych wpisów potrzebna jest rola ``ROLE_ADMIN``.
Zastosowania ``@Security``, wygląda tak:

.. code-block:: php
   :linenos:

    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Security;
    // ...

    /**
     * Displays a form to create a new Post entity.
     *
     * @Route("/new", name="admin_post_new")
     * @Security("has_role('ROLE_ADMIN')")
     */
    public function newAction()
    {
        // ...
    }

Wykorzystywanie wyrażeń dla złożonych ograniczeń dostępu
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Jeśli logika zabeczeń jest trochę bardziej złożona, można użyć `wyrażenia`_
wewnątrz adnotacji ``@Security``. W poniższym przykładzie, użytkownik może tylko
mieć dostęp do kontrolera, jeśli adres email jest zgodny z wartością zwracana przez
metodę ``getAuthorEmail`` na obiekcie ``Post``:

.. code-block:: php
   :linenos:

    use AppBundle\Entity\Post;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Security;

    /**
     * @Route("/{id}/edit", name="admin_post_edit")
     * @Security("user.getEmail() == post.getAuthorEmail()")
     */
    public function editAction(Post $post)
    {
        // ...
    }

Należy mieć na uwadze, że wymaga to użycia `ParamConverter`_, który automatycznie
wypytuje o obiekt ``Post`` i wstawia go do argumentu ``$post``. Jest to tym, co
sprawia, że jest możliwe użycie w wyrażeniu zmiennej ``post``.

Ma to jedną poważną wadę: wyrażenie w adnotacji nie można tak łatwo użyć ponownie
w innych częściach aplikacji. Wyobraźmy sobie, że chcemy dodać odnośnik w szablonie,
który ma być tylko widoczny dla autorów. Niestety, teraz trzeba powtórzyć ten
kod wyrażenia, stosując składnię Twig:

.. code-block:: html+jinja
   :linenos:

    {% if app.user and app.user.email == post.authorEmail %}
        <a href=""> ... </a>
    {% endif %}

Najprostrzym rozwiązaniem jest, jeśli logika jest dość prosta, dodanie nowej metody
do encji ``Post``, która sprawdza czy dany użytkownik jest autorem:

.. code-block:: php
   :linenos:

    // src/AppBundle/Entity/Post.php
    // ...

    class Post
    {
        // ...

        /**
         * Is the given User the author of this Post?
         *
         * @return bool
         */
        public function isAuthor(User $user = null)
        {
            return $user && $user->getEmail() == $this->getAuthorEmail();
        }
    }

Teraz można wykorzystać wielokrotnie tą metodę, zarówno w szablonie, jak i w wyrażeniu
zabezpieczającym:

.. code-block:: php
   :linenos:

    use AppBundle\Entity\Post;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Security;

    /**
     * @Route("/{id}/edit", name="admin_post_edit")
     * @Security("post.isAuthor(user)")
     */
    public function editAction(Post $post)
    {
        // ...
    }

.. code-block:: html+jinja
   :linenos:

    {% if post.isAuthor(app.user) %}
        <a href=""> ... </a>
    {% endif %}

.. index::
   single: bezpieczeństwo; indywidualny kod autoryzacyjny
   single: autoryzacja; indywidualny kod autoryzacyjny

.. _best-practices-directly-isGranted:
.. _checking-permissions-without-security:
.. _manually-checking-permissions:

Sprawdzanie uprawnień bez adnotacji @Security
---------------------------------------------

Powyższy przykład z wykorzystaniem adnotacji ``@Security`` dziala tylko dlatego,
że użyliśmy :ref:`ParamConverter <best-practices-paramconverter>`, który podaje
wyrażenie z regułą dostępu do zmiennej ``post``. Jeśli nie chcesz z tego korzystać,
lub masz bardziej zaawansowany przypadek, możesz zawsze wykonać indywidualny kod
autoryzacyjny w PHP:

.. code-block:: php
   :linenos:

    /**
     * @Route("/{id}/edit", name="admin_post_edit")
     */
    public function editAction($id)
    {
        $post = $this->getDoctrine()->getRepository('AppBundle:Post')
            ->find($id);

        if (!$post) {
            throw $this->createNotFoundException();
        }

        if (!$post->isAuthor($this->getUser())) {
            $this->denyAccessUnlessGranted('edit', $post);

	    // or without the shortcut:
	    //
	    // use Symfony\Component\Security\Core\Exception\AccessDeniedException;
	    // ...
	    //
	    // if (!$this->get('security.authorization_checker')->isGranted('edit', $post)) {
	    //    throw $this->createAccessDeniedException();
	    // }
        }

        // ...
    }

.. index::
   single: bezpieczeństwo; wyborcy autoryzacji
   single: autoryzacja; wyborcy


Wyborcy autoryzacji
-------------------

Jeśli logika autoryzacji jest skomplikowana i nie może zostać scentralizowana
w metodzie takiej jak ``isAuthor()``, należy wykorzystać własnych wyborców
(*ang. voters*). Jest to znacznie łatwiejsze niż :doc:`listy ACLs </cookbook/security/acl>`
i daje elastyczność prawie w każdym przypadku.

Po pierwsze trzeba utworzyć klasę wyborcy. W poniższym przykładzie pokazano
wyborcę. który implementuje tą sama logikę ``getAuthorEmail``, jaką użyto poprzednio:

.. code-block:: php
   :linenos:

    namespace AppBundle\Security;

    use Symfony\Component\Security\Core\Authorization\Voter\AbstractVoter;
    use Symfony\Component\Security\Core\User\UserInterface;

    // AbstractVoter class requires Symfony 2.6 or higher version
    class PostVoter extends AbstractVoter
    {
        const CREATE = 'create';
        const EDIT   = 'edit';

        protected function getSupportedAttributes()
        {
            return array(self::CREATE, self::EDIT);
        }

        protected function getSupportedClasses()
        {
            return array('AppBundle\Entity\Post');
        }

        protected function isGranted($attribute, $post, $user = null)
        {
            if (!$user instanceof UserInterface) {
                return false;
            }

            if ($attribute === self::CREATE && in_array('ROLE_ADMIN', $user->getRoles(), true)) {
                return true;
            }

            if ($attribute === self::EDIT && $user->getEmail() === $post->getAuthorEmail()) {
                return true;
            }

            return false;
        }
    }

Do włączenia wyborcy w aplikacji, trzeba zdefiniować nową usługę:

.. code-block:: yaml
   :linenos:

    # app/config/services.yml
    services:
        # ...
        post_voter:
            class:      AppBundle\Security\PostVoter
            public:     false
            tags:
               - { name: security.voter }

Teraz, można wykorzystywać wyborcę w adnotacji ``@Security``:

.. code-block:: php
   :linenos:

    /**
     * @Route("/{id}/edit", name="admin_post_edit")
     * @Security("is_granted('edit', post)")
     */
    public function editAction(Post $post)
    {
        // ...
    }

Można to również użyć bezpośrednio w usłudze ``security.authorization_checker``
lub przez jeszcze łatwiejszy skrót w akcji kontrolera:

.. code-block:: php
   :linenos:

    /**
     * @Route("/{id}/edit", name="admin_post_edit")
     */
    public function editAction($id)
    {
        $post = // query for the post ...

        $this->denyAccessUnlessGranted('edit', $post);

        // or without the shortcut:
        //
        // use Symfony\Component\Security\Core\Exception\AccessDeniedException;
        // ...
        //
        // if (!$this->get('security.authorization_checker')->isGranted('edit', $post)) {
        //    throw $this->createAccessDeniedException();
        // }
    }

Dalsza lektura
--------------

Pakiet `FOSUserBundle`_, stworzony przez społeczność Symfony, zapewnia obsługę
użytkowników z wykorzystaniem bazy danych. Obsługuje również kilka popularnych
funkcjonalności, takich jak rejestracja użytkowników i resetowanie hasła.

Włącz w swojej aplikacji funkcjonalność :doc:`"zapamiętaj mnie" </cookbook/security/remember_me>`,
co umożliwi użytkownikom pozostawania zalogowanym przez dłuższy okres czasu.

W przypadku dostarczania obsługi klientów, czasem moze być konieczne uzyskanie
dostęþu do aplikacji jako jakiś *inny* użytkownik, aby rozwiazać problem zgłaszany
przez tego klienta. Symfony dostarcza w tym celu możliwość
:doc:`podszywania się pod użytkownika </cookbook/security/impersonating_user>`.

Jeśli firma używa metodę logowania nie obsługiwaną przez Symfony, można zaprojektować
:doc:`własnego dostawcę użytkowników </cookbook/security/custom_provider>` i
:doc:`własnego dostawcę uwierzytelniania </cookbook/security/custom_authentication_provider>`.

.. _`ParamConverter`: http://symfony.com/doc/current/bundles/SensioFrameworkExtraBundle/annotations/converters.html
.. _`@Security annotation`: http://symfony.com/doc/current/bundles/SensioFrameworkExtraBundle/annotations/security.html
.. _`wyrażenia`: http://symfony.com/doc/current/components/expression_language/introduction.html
.. _`FOSUserBundle`: https://github.com/FriendsOfSymfony/FOSUserBundle
