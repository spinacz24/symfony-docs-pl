.. index::
   single: Doctrine; formularz rejestracyjny
   single: formularze; formularz rejestracyjny

Jak zaimplementować prosty formularz rejestracyjny
==================================================

Niektóre formularze mają dodatkowe pola, których wartość nie musi być przechowywana
w bazie danych. Na przykład, można utworzyć formularz rejestracyjny z jakimiś
dodatkowymi polami (takimi jak pole wyboru "zaakceptowanie Regulaminu") i osadzić
w nim formularz, który rzeczywiście przechowuje informacje o koncie.

Prosty model User
-----------------

Stwórzmy prostą encję ``User`` odwzorowywaną w bazie danych::

    // src/Acme/AccountBundle/Entity/User.php
    namespace Acme\AccountBundle\Entity;

    use Doctrine\ORM\Mapping as ORM;
    use Symfony\Component\Validator\Constraints as Assert;
    use Symfony\Bridge\Doctrine\Validator\Constraints\UniqueEntity;

    /**
     * @ORM\Entity
     * @UniqueEntity(fields="email", message="Email already taken")
     */
    class User
    {
        /**
         * @ORM\Id
         * @ORM\Column(type="integer")
         * @ORM\GeneratedValue(strategy="AUTO")
         */
        protected $id;

        /**
         * @ORM\Column(type="string", length=255)
         * @Assert\NotBlank()
         * @Assert\Email()
         */
        protected $email;

        /**
         * @ORM\Column(type="string", length=255)
         * @Assert\NotBlank()
         * @Assert\Length(max = 4096)
         */
        protected $plainPassword;

        public function getId()
        {
            return $this->id;
        }

        public function getEmail()
        {
            return $this->email;
        }

        public function setEmail($email)
        {
            $this->email = $email;
        }

        public function getPlainPassword()
        {
            return $this->plainPassword;
        }

        public function setPlainPassword($password)
        {
            $this->plainPassword = $password;
        }
    }

Encja ``User`` zawiera trzy pola a dwa z nich (``email`` i ``plainPassword``)
trzeba wyświetlić w formularzu. Właściwość adresu email musi być unikalna
w bazie danych, co jest wymuszane przez dodanie walidacji na początku klasy.

.. note::

    Jeśli chce się zintegrować encję User z systemem bezpieczeństwa, trzeba
    zaimplementować :ref:`UserInterface <book-security-user-entity>` komponentu
    Security.

.. _cookbook-registration-password-max:

.. sidebar:: Dlaczego ograniczenie hasła do 4096 znaków?

    Proszę zauważyć, że pole ``plainPassword`` ma maksymalną długość 4096 znaków.
    W celach bezpieczeństwa (`CVE-2013-5750`_), Symfony ogranicza długość hasła
    tekstowego do 4096, podczas kodowania go. Dodając to ograniczenie, trzeba
    obsłużyć błąd powstający, gdy ktoś będzie próbował wprowadzić super długie
    hasło.

    Ograniczenie to trzeba dodać w dowolnym miejscu aplikacji, gdzie użytkownik
    wprowadza hasło w zwykłym tekście (np. w formularzu zmiany hasła). Jedynym
    miejsce, gdzie nie trzeba się o to martwić jest formularz logowania,
    ponieważ komponent Security Symfony obsługuje to sam.

Utworzenie formularza dla modelu User
-------------------------------------

Następnie, trzeba utworzyć formularz dla modelu ``User``::

    // src/Acme/AccountBundle/Form/Type/UserType.php
    namespace Acme\AccountBundle\Form\Type;

    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\FormBuilderInterface;
    use Symfony\Component\OptionsResolver\OptionsResolver;

    class UserType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder->add('email', 'email');
            $builder->add('plainPassword', 'repeated', array(
               'first_name'  => 'password',
               'second_name' => 'confirm',
               'type'        => 'password',
            ));
        }

        public function configureOptions(OptionsResolver $resolver)
        {
            $resolver->setDefaults(array(
                'data_class' => 'Acme\AccountBundle\Entity\User'
            ));
        }

        public function getName()
        {
            return 'user';
        }
    }

Istnieją tylko dwa pola: ``email`` i ``plainPassword`` (powtórzone w celu potwierdzenia
wprowadzanego konta). Opcja ``data_class`` informule formularz o nazwie podstawowej
klasy danych (czyli o encji ``User``).

.. tip::

    W celu poznania więcej rzeczy o komponencie Form, proszę przeczytać :doc:`/book/forms`.

Osadzenie formularza User w formularzu rejestracyjnym
-----------------------------------------------------

Formularz, który będziemy uzywac do rejestracji nie jest tym samym formularzem,
jaki zdefiniowalismy w typie ``User`` (czyli ``UserType``). Formularz rejestracyjny
bedzie zawierał dodatkowe pola , takie jak "zaakceptowanie Regulaminu", którego
wartość nie będzie przechowywana w bazie danych.

Rozpoczniemy od utworzenia prostej klasy reprezentującej "rejestrację"::

    // src/Acme/AccountBundle/Form/Model/Registration.php
    namespace Acme\AccountBundle\Form\Model;

    use Symfony\Component\Validator\Constraints as Assert;

    use Acme\AccountBundle\Entity\User;

    class Registration
    {
        /**
         * @Assert\Type(type="Acme\AccountBundle\Entity\User")
         * @Assert\Valid()
         */
        protected $user;

        /**
         * @Assert\NotBlank()
         * @Assert\True()
         */
        protected $termsAccepted;

        public function setUser(User $user)
        {
            $this->user = $user;
        }

        public function getUser()
        {
            return $this->user;
        }

        public function getTermsAccepted()
        {
            return $this->termsAccepted;
        }

        public function setTermsAccepted($termsAccepted)
        {
            $this->termsAccepted = (bool) $termsAccepted;
        }
    }

Następnie, utworzymy formularz dla modelu ``Registration``::

    // src/Acme/AccountBundle/Form/Type/RegistrationType.php
    namespace Acme\AccountBundle\Form\Type;

    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\FormBuilderInterface;

    class RegistrationType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder->add('user', new UserType());
            $builder->add(
                'terms',
                'checkbox',
                array('property_path' => 'termsAccepted')
            );
            $builder->add('Register', 'submit');
        }

        public function getName()
        {
            return 'registration';
        }
    }

Nie musimy użyć specjalnej metody do osadzania formularza ``UserType``.
Jakiś formularz może zostać potraktowany jako pole innego formularza, tak więc
możemy dodać ``UserType`` jak inne pola, sprawiając, że właściwość ``Registration.user``
bedzie instancją klasy ``User``.

Obsługa zgłaszania formularza
-----------------------------

Teraz, musimy wykonać kontroler obsługujacy formularz. Rozpoczniemy od utworzenia
prostego kontrolera wyświetlajacego formularz rejestracyjny::

    // src/Acme/AccountBundle/Controller/AccountController.php
    namespace Acme\AccountBundle\Controller;

    use Symfony\Bundle\FrameworkBundle\Controller\Controller;

    use Acme\AccountBundle\Form\Type\RegistrationType;
    use Acme\AccountBundle\Form\Model\Registration;

    class AccountController extends Controller
    {
        public function registerAction()
        {
            $registration = new Registration();
            $form = $this->createForm(new RegistrationType(), $registration, array(
                'action' => $this->generateUrl('account_create'),
            ));

            return $this->render(
                'AcmeAccountBundle:Account:register.html.twig',
                array('form' => $form->createView())
            );
        }
    }

oraz jego szablon:

.. code-block:: html+jinja

    {# src/Acme/AccountBundle/Resources/views/Account/register.html.twig #}
    {{ form(form) }}

Po czym, stworzymy akcję obsługujaca zgłaszanie formularza. Wykonuje ona walidację
i zapisuje dane w bazie danych::

    use Symfony\Component\HttpFoundation\Request;
    // ...

    public function createAction(Request $request)
    {
        $em = $this->getDoctrine()->getManager();

        $form = $this->createForm(new RegistrationType(), new Registration());

        $form->handleRequest($request);

        if ($form->isValid()) {
            $registration = $form->getData();

            $em->persist($registration->getUser());
            $em->flush();

            return $this->redirectToRoute(...);
        }

        return $this->render(
            'AcmeAccountBundle:Account:register.html.twig',
            array('form' => $form->createView())
        );
    }

Dodanie nowych tras
-------------------

Konieczne jest teraz zaktualizowanie tras. Jeśli umieścisz trasy w swoim pakiecie,
(tak jak pokazano to tutaj), upewnij się, że plik trasowania został 
:ref:`zaimportowany <routing-include-external-resources>`.

.. configuration-block::

    .. code-block:: yaml

        # src/Acme/AccountBundle/Resources/config/routing.yml
        account_register:
            path:     /register
            defaults: { _controller: AcmeAccountBundle:Account:register }

        account_create:
            path:     /register/create
            defaults: { _controller: AcmeAccountBundle:Account:create }

    .. code-block:: xml

        <!-- src/Acme/AccountBundle/Resources/config/routing.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="account_register" path="/register">
                <default key="_controller">AcmeAccountBundle:Account:register</default>
            </route>

            <route id="account_create" path="/register/create">
                <default key="_controller">AcmeAccountBundle:Account:create</default>
            </route>
        </routes>

    .. code-block:: php

        // src/Acme/AccountBundle/Resources/config/routing.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('account_register', new Route('/register', array(
            '_controller' => 'AcmeAccountBundle:Account:register',
        )));
        $collection->add('account_create', new Route('/register/create', array(
            '_controller' => 'AcmeAccountBundle:Account:create',
        )));

        return $collection;

Zaktualizowanie schematu bazy danych
------------------------------------

Ponieważ dodaliśmy encję ``User``, trzeba zaktualizować schemat bazy danych:

.. code-block:: bash

   $ php app/console doctrine:schema:update --force

Nasz formularz zostanie sprawdzony a obiekt ``User`` utrwalony w bazie danych.
Dodatkowe pole wyboru ``terms`` w klase modelu ``Registration`` jest używane
tylko podczas walidowania formularza i nie jest później wykorzystywane podczas
zapisu obiektu User do bazy danych.

.. _`CVE-2013-5750`: https://symfony.com/blog/cve-2013-5750-security-issue-in-fosuserbundle-login-form
