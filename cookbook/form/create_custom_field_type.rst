.. index::
   single: Form; Własny typ pola formularza

Jak utworzyć własny typ pola formularza
======================================

Symfony domyślnie posiada kilka podstawowych typów pól do budowy formularzy.
Jednak są sytuacje kiedy to nie wystarcza i potrzebujemy stworzyć własny typ do specyficznych
rozwiązań. W tym receptariuszu bazując na podstawowym polu choice 
utworzymy definicję typu pola do wyboru płci. Wyjaśnimy w 
jaki sposób definiować typy pól, jak dostosować ich wygląd końcowy oraz 
jak zarejestrować do użytku w aplikacji.

Definiowanie typu pola
-----------------------

W pierwszej kolejności musimy utworzyć klasę reprezentującą nowy typ pola. 
W na potrzeby tego wpisu klasa ta będzie sie nazywała ``GenderType`` 
i umieszczona będzie w domyślnej lokalizacji dla niestandardowych typów pól 
``<BundleName>\Form\Type``. Upewnij się że klasa dziedziczy po
:class:`Symfony\\Component\\Form\\AbstractType`::

    // src/AppBundle/Form/Type/GenderType.php
    namespace AppBundle\Form\Type;

    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\OptionsResolver\OptionsResolver;

    class GenderType extends AbstractType
    {
        public function configureOptions(OptionsResolver $resolver)
        {
            $resolver->setDefaults(array(
                'choices' => array(
                    'm' => 'Male',
                    'f' => 'Female',
                )
            ));
        }

        public function getParent()
        {
            return 'choice';
        }

        public function getName()
        {
            return 'gender';
        }
    }

.. tip::
    
    Lokalizacja w której umieszczamy klasę nowego typu pola ``<BundleName>\Form\Type``, nie jest 
    restrykcyjna, ale jest to bardzo dobra konwencja.

Metoda ``getParent`` zwraca ``choice``, informuje to Symfony który
typ pola chcemy rozszerzać. Oznacza to, że będziemy dziedziczyć całą logikę, oraz
sposób renderowania tego typu pola. Aby dowiedzieć się więcej na temat sposobu działania pola ``choice``,
zapoznaj się z klasą `ChoiceType`_. Występują tam trzy szczególnie ważne metody:

``buildForm()``
    Każdy typ formularza posiada metodę ``buildForm``, w której konfigurujesz i budujesz 
    dowolne pola. Proszę zauważyć, że jest to podobna metoda którą używa się
    do budowy formularzy i tutaj działa w taki sam sposób.

``buildView()``
    Ta metoda używana jest do ustawiania dodatkowych zmiennych które możesz wykorzystać
    podczas renderowania pola w szablonie. Dla przykładu, w polu `ChoiceType`_, zmienna
    ``multiple`` używana jest w szablonie do ustawiania atrybutu ``multiple`` w polu ``select``.
    Po więcej informacji, zapoznaj się z `Creating a Template for the Field`_.

.. versionadded:: 2.7
    Metoda ``configureOptions()`` została wporwadzona w Symfony 2.7. Wcześniej
    nazywała się ``setDefaultOptions()``.

``configureOptions()``
    Metoda definiuje opcje dla Twojego typu formularza które mogą być wykorzystane w 
    ``buildForm()`` i ``buildView()``. Standardowo jest wiele wspólnych opcji dla każdego pola
    (zobacz :doc:`/reference/forms/types/form`), jeżeli potrzebujesz nowych opcji,
    musisz dodać je tutaj.

.. tip::

    Jeżeli tworzysz typ pola zwierający wiele innych pól, upewnij się że metoda getParent()
    zwraca typ ``form``, lub po innym typ który rozszerza ``form``.
    Ponadto jeżeli chcesz zmodyfikować widoki dla jakiegokolwiek pola podrzędnego 
    to dla nadrzędnego typu pola formularza użyj metody ``finishView()``.

Metoda ``getName()`` powinna zwracać unikalny identyfikator.
Jest to używane do rejestracji typu pola lub do nadpisywania w szablonie formularza
odpowiednich bloków, aby dostosować renderowanie pola.

W tym artykule, naszym celem jest rozszerzenie pola choice aby umożliwić wybór płci.
Uzyskamy taką możliwość poprzez ustalenie stałej listy płci w polu ``choices``.

Tworzenie szablonu dla nowego typu pola.
---------------------------------

Każdy typ pola jest renderowany dzięki odpowiednim fragmentom szablonu które można 
nadpisać poprzez odpowiedni identyfikator pola, zwracany w metodzie ``getName()``.
Więcej informacji znajdziesz w artykule :ref:`cookbook-form-customization-form-themes`.

W tym przypadku, odkąd polem nadrzędnym jest ``choice``, *nie musisz* wykonywać
żadnej dodatkowej pracy aby pole mogło być renderowane, ponieważ sposób 
renderowania odziedziczy po type ``choice``.

Dla tego przypadku, można się spodziewać, że typ pola będzie renderowany zawsze jako
kontrolki radio lub checkbox'y, zamiast kontrolki select, chcemy również aby były otoczone znacznikiem ``ul``. 
W pliku szablonu odpowiedzialnym za renderowanie typu pola (zobacz :ref:`cookbook-form-customization-form-themes`), 
utwórz blok ``gender_widget``.

.. configuration-block::

    .. code-block:: html+jinja

        {# src/AppBundle/Resources/views/Form/fields.html.twig #}
        {% block gender_widget %}
            {% spaceless %}
                {% if expanded %}
                    <ul {{ block('widget_container_attributes') }}>
                    {% for child in form %}
                        <li>
                            {{ form_widget(child) }}
                            {{ form_label(child) }}
                        </li>
                    {% endfor %}
                    </ul>
                {% else %}
                    {# just let the choice widget render the select tag #}
                    {{ block('choice_widget') }}
                {% endif %}
            {% endspaceless %}
        {% endblock %}

    .. code-block:: html+php

        <!-- src/AppBundle/Resources/views/Form/gender_widget.html.php -->
        <?php if ($expanded) : ?>
            <ul <?php $view['form']->block($form, 'widget_container_attributes') ?>>
            <?php foreach ($form as $child) : ?>
                <li>
                    <?php echo $view['form']->widget($child) ?>
                    <?php echo $view['form']->label($child) ?>
                </li>
            <?php endforeach ?>
            </ul>
        <?php else : ?>
            <!-- just let the choice widget render the select tag -->
            <?php echo $view['form']->renderBlock('choice_widget') ?>
        <?php endif ?>

.. note::

    Upewnij sie że użyłeś prefiks widget. W tym przykładzie nazwa powinna wyglądać
    następująco ``gender_widget``, jest to zależne od wartości zwracanej w metodzie ``getName``.
    Następnie, główny plik konfiguracyjny powinien wskazywać na szablon który będzie
    wykorzystywany do renderowania formularza z nowym typem pola.

    Twig:

    .. configuration-block::

        .. code-block:: yaml

            # app/config/config.yml
            twig:
                form_themes:
                    - 'AppBundle:Form:fields.html.twig'

        .. code-block:: xml

            <!-- app/config/config.xml -->
            <twig:config>
                <twig:form-theme>AppBundle:Form:fields.html.twig</twig:form-theme>
            </twig:config>

        .. code-block:: php

            // app/config/config.php
            $container->loadFromExtension('twig', array(
                'form_themes' => array(
                    'AppBundle:Form:fields.html.twig',
                ),
            ));

    Szablon w PHP

    .. configuration-block::

        .. code-block:: yaml

            # app/config/config.yml
            framework:
                templating:
                    form:
                        resources:
                            - 'AppBundle:Form'

        .. code-block:: xml

            <!-- app/config/config.xml -->
            <?xml version="1.0" encoding="UTF-8" ?>
            <container xmlns="http://symfony.com/schema/dic/services"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xmlns:framework="http://symfony.com/schema/dic/symfony"
                xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

                <framework:config>
                    <framework:templating>
                        <framework:form>
                            <framework:resource>AppBundle:Form</twig:resource>
                        </framework:form>
                    </framework:templating>
                </framework:config>
            </container>

        .. code-block:: php

            // app/config/config.php
            $container->loadFromExtension('framework', array(
                'templating' => array(
                    'form' => array(
                        'resources' => array(
                            'AppBundle:Form',
                        ),
                    ),
                ),
            ));

Użycie typu pola
--------------------

Teraz możesz odrazy użyć nowego typu pola, wystarczy że stworzysz instancje w Twoim formularzu:: 

    // src/AppBundle/Form/Type/AuthorType.php
    namespace AppBundle\Form\Type;

    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\FormBuilderInterface;

    class AuthorType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder->add('gender_code', new GenderType(), array(
                'placeholder' => 'Choose a gender',
            ));
        }
    }

W tym przypadku wszystko działa, ponieważ ``GenderType()`` jest bardzo prosty. A co
gdyby indentyfikator typu pola utrzymywany był w konfiguracji lub w bazie?
Następna sekcja, wyjaśni jak bardziej złożone typy pól rozwiązują ten problem.

.. versionadded:: 2.6
    Opcja ``placeholder`` została wprowadzona w Symfony 2.6 na rzecz
    ``empty_value``, która jest dostępna przed 2.6.

.. _form-cookbook-form-field-service:

Tworzenie nowego typu pola jako usługę
-------------------------------------

Do tej pory, ten wpis zakładał, że wykonamy bardzo prosty nowy typ pola .
Ale jeżeli będziesz potrzebował dostępu do konfiguracji, połączenia z bazą, albo innych usług,
wtedy będziesz należy zarejestrować tworzony typ pola jako usługę. Dla przykładu,
załóżmy że przechowujemy parametry płci w konfiguracji.

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        parameters:
            genders:
                m: Male
                f: Female

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <parameters>
            <parameter key="genders" type="collection">
                <parameter key="m">Male</parameter>
                <parameter key="f">Female</parameter>
            </parameter>
        </parameters>

    .. code-block:: php

        // app/config/config.php
        $container->setParameter('genders.m', 'Male');
        $container->setParameter('genders.f', 'Female');

Aby używać parametrów z konfiguracji, zdefiniuj swój typ pola jako usługę, wstrzyknij paramter
``genders`` jako argument który będzie przekazany do metody ``__construct``

.. configuration-block::

    .. code-block:: yaml

        # src/AppBundle/Resources/config/services.yml
        services:
            app.form.type.gender:
                class: AppBundle\Form\Type\GenderType
                arguments:
                    - "%genders%"
                tags:
                    - { name: form.type, alias: gender }

    .. code-block:: xml

        <!-- src/AppBundle/Resources/config/services.xml -->
        <service id="app.form.type.gender" class="AppBundle\Form\Type\GenderType">
            <argument>%genders%</argument>
            <tag name="form.type" alias="gender" />
        </service>

    .. code-block:: php

        // src/AppBundle/Resources/config/services.php
        use Symfony\Component\DependencyInjection\Definition;

        $container
            ->setDefinition('app.form.type.gender', new Definition(
                'AppBundle\Form\Type\GenderType',
                array('%genders%')
            ))
            ->addTag('form.type', array(
                'alias' => 'gender',
            ))
        ;

.. tip::

    Upewnij się, że plik z Twoimi usługami importowany. Zapoznaj się z
    :ref:`service-container-imports-directive`

Upewnij się czy wartość klucza ``alias`` odpowiada wartości zwracanej przez metodę ``getName``.
Teraz dodaj metodę ``__construct`` do klasy ``GenderType``, gdzie odbierzemy parametr płci ``genders``::

    // src/AppBundle/Form/Type/GenderType.php
    namespace AppBundle\Form\Type;

    use Symfony\Component\OptionsResolver\OptionsResolver;

    // ...

    // ...
    class GenderType extends AbstractType
    {
        private $genderChoices;

        public function __construct(array $genderChoices)
        {
            $this->genderChoices = $genderChoices;
        }

        public function configureOptions(OptionsResolver $resolver)
        {
            $resolver->setDefaults(array(
                'choices' => $this->genderChoices,
            ));
        }

        // ...
    }

Świetnie! Klasa ``GenderType`` jest uzupełniona parametrem z konfiguracji 
oraz zarejestrowaliśmy ją jako usługę. Dodatkowo, ponieważ stosujesz ``form.type``
w konfiguracji usługi, używanie naszego pola staje się teraz znacznie prostsze::

    // src/AppBundle/Form/Type/AuthorType.php
    namespace AppBundle\Form\Type;

    use Symfony\Component\Form\FormBuilderInterface;

    // ...

    class AuthorType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder->add('gender_code', 'gender', array(
                'placeholder' => 'Choose a gender',
            ));
        }
    }

Zauważ, że zamiast tworzyć nową instancję, można teraz po prostu odnieść się poprzez
alias wprowadzony w konfiguracji usług, ``gender``. Baw się dobrze!

.. _`ChoiceType`: https://github.com/symfony/symfony/blob/master/src/Symfony/Component/Form/Extension/Core/Type/ChoiceType.php
.. _`FieldType`: https://github.com/symfony/symfony/blob/master/src/Symfony/Component/Form/Extension/Core/Type/FieldType.php
