.. index::
   single: Form; Rozszerzanie typów formularza

Jak rozszerzać typy formularza
===================================

:doc:`Niestandardowe typy pól formularza <create_custom_field_type>` są świetne,
kiedy potrzebujesz specyficznego typu pola, na przykład selektor płci lub pole z wartością podatku VAT.

Jednak nie zawsze potrzebujemy tworzyć coś od nowa, czasami wystarczyłoby 
dopisać funkcjonalności do istniejących pól. Do takich zadań służą rozszerzenia typów pól.

Rozszerzenia wykorzystujemy w dwóch głównych przypadkach:

#. Chcemy dodać **generyczną funkcjonalność do kilku typów** (przykład, dodanie „help” do każdego typu pola)
#. Chcemy dodać **specyficzną funkcjonalność do pojedynczego pola** (przykład, dodanie  funkcjonalności „download” do „file field type”)

W obu przypadkach, cel być może osiągniemy poprzez zmodyfikowania w szablonie bloku
formularza lub utworzenie niestandardowego typu pola. 
Dla uniknięcia niepotrzebnej nadmiarowości kodu oraz zwiększenia elastyczności, 
możemy użyć rozszerzenia typów (można dodawać kilka rozszerzeń do danego typu formularza).

Za pomocą rozszerzeń typów formularza można osiągnąć większość tego, 
co osiągnęlibyśmy dziedzicząc po danym typie pola, ale zamiast tworzyć nowe typy pól, 
**można rozszerzać istniejące**.

Wyobraź sobie że zarządzasz encją ``Media``, gdzie do każdej takiej encji przypisany jest plik.
Formularz ``Media`` używa pola typu file, kiedy edytujesz encję chciałbyś aby automatycznie,
przy polu file wyświetlane było zdjęcie.

Oczywiście mógłbyś dostosować to w jaki sposób ma być wyświetlane pole w samym szablonie.
Rozszerzenia typów pól, pozwalają Ci wykonać to w bardziej elegancki sposób.

Definiowanie rozszerzenia typu pola
--------------------------------

Twoim pierwszym zadaniem będzie utworzenie klasy rozszerzenia typu pola (w przypadku tego 
artykuły będzie to ``ImageTypeExtension``). Dobrą praktyką jest umieszczanie rozszerzeń typów pól
w folderze pakietu pod ścieżką ``Form\Extension``.

Podczas tworzenia rozszerzenia typu pola, możesz albo zaimplementować interfejs 
:class:`Symfony\\Component\\Form\\FormTypeExtensionInterface` lub odziedziczyć klasę abstrakcyjną 
:class:`Symfony\\Component\\Form\\AbstractTypeExtension`. W większości przypadkach, 
łatwiej będzie odziedziczyć klasę abstrakcyjną::

    // src/Acme/DemoBundle/Form/Extension/ImageTypeExtension.php
    namespace Acme\DemoBundle\Form\Extension;

    use Symfony\Component\Form\AbstractTypeExtension;

    class ImageTypeExtension extends AbstractTypeExtension
    {
        /**
         * Returns the name of the type being extended.
         *
         * @return string The name of the type being extended
         */
        public function getExtendedType()
        {
            return 'file';
        }
    }

Jedyną metodą jaką **musisz** zaimplementować to ``getExtendedType``.
Używana jest do wskazania jaki typ formularza będzie rozszerzała Twoja klasa.

.. tip::

    Wartość zwracana przez metodę ``getExtendedType`` musi być identyczna z
    wartością zwracaną poprzez metodę ``getName`` klasy formularza który chcesz rozszerzać.

Dodatkowo oprócz metody ``getExtendedType``, prawdopodobnie będziesz chciał nadpisać następujące metody:

* ``buildForm()``

* ``buildView()``

* ``configureOptions()``

* ``finishView()``

Po więcej informacji dotyczących tych metody, odsyłam do artykułu receptariusza
:doc:`Tworzenie niestandardowych typów formularza </cookbook/form/create_custom_field_type>`

Rejestrowanie rozszerzenia typu formularza jako serwis
-------------------------------------------------

Następnym krokiem jest poinformowanie Symfony o Twoim rozszerzeniu. Wszystko co musisz zrobić
sprowadza się do deklaracji serwisu używając taga ``form.type_extension``

tag:

.. configuration-block::

    .. code-block:: yaml

        services:
            acme_demo_bundle.image_type_extension:
                class: Acme\DemoBundle\Form\Extension\ImageTypeExtension
                tags:
                    - { name: form.type_extension, alias: file }

    .. code-block:: xml

        <service id="acme_demo_bundle.image_type_extension"
            class="Acme\DemoBundle\Form\Extension\ImageTypeExtension"
        >
            <tag name="form.type_extension" alias="file" />
        </service>

    .. code-block:: php

        $container
            ->register(
                'acme_demo_bundle.image_type_extension',
                'Acme\DemoBundle\Form\Extension\ImageTypeExtension'
            )
            ->addTag('form.type_extension', array('alias' => 'file'));


Klucz ``alias`` tagu jest nazwą typu pola którą chcesz rozszerzyć.
Jeżeli chcesz rozszerzyć typ pola ``file``, należy użyć ``file`` jako alias.

Dodanie logiki biznesowej do rozszerzenia 
-----------------------------------
Celem Twojego rozszerzenia w tym artykule jest wyświetlenie obrazka obok pola file
(kiedy encja zawiera obrazek). W tym przypadku, zakładamy że masz podobne podejście jak w artykule
:doc:`Jak obsłużyć Wysyłanie Plików z Doctrine </cookbook/doctrine/file_uploads>`:
posiadasz encję Media z właściwością file (odpowiadającej w formularzu polu w postaci pliku) oraz
właściwość path (odpowiadająca ścieżce do zdjęcia przechowywanej w Twojej bazie)::

    // src/Acme/DemoBundle/Entity/Media.php
    namespace Acme\DemoBundle\Entity;

    use Symfony\Component\Validator\Constraints as Assert;

    class Media
    {
        // ...

        /**
         * @var string The path - typically stored in the database
         */
        private $path;

        /**
         * @var \Symfony\Component\HttpFoundation\File\UploadedFile
         * @Assert\File(maxSize="2M")
         */
        public $file;

        // ...

        /**
         * Get the image URL
         *
         * @return null|string
         */
        public function getWebPath()
        {
            // ... $webPath being the full image URL, to be used in templates

            return $webPath;
        }
    }

Aby rozszerzyć typ pola ``file`` Twoje rozszerzenie w pierwszej 
kolejności potrzebuje dwóch rzeczy:

#. Metodę ``configureOptions`` w celu dodania opcji ``image_path``
#. Metody ``buildForm`` i ``buildView`` w celu przekazania URL zdjęcia do widoku.

Logika działania jest następująca: kiedy dodamy do formularza pole typu ``file``, 
będziesz miał możliwość zastosowania nowej opcji: ``image_path``. Dzięki tej opcji pole 
``file`` będzie wiedziało w jaki jest URL obrazka do wyświetlenia go w widoku::

    // src/Acme/DemoBundle/Form/Extension/ImageTypeExtension.php
    namespace Acme\DemoBundle\Form\Extension;

    use Symfony\Component\Form\AbstractTypeExtension;
    use Symfony\Component\Form\FormView;
    use Symfony\Component\Form\FormInterface;
    use Symfony\Component\PropertyAccess\PropertyAccess;
    use Symfony\Component\OptionsResolver\OptionsResolverInterface;

    class ImageTypeExtension extends AbstractTypeExtension
    {
        /**
         * Returns the name of the type being extended.
         *
         * @return string The name of the type being extended
         */
        public function getExtendedType()
        {
            return 'file';
        }

        /**
         * Add the image_path option
         *
         * @param OptionsResolver $resolver
         */
        public function configureOptions(OptionsResolver $resolver)
        {
            $resolver->setOptional(array('image_path'));
        }

        /**
         * Pass the image URL to the view
         *
         * @param FormView $view
         * @param FormInterface $form
         * @param array $options
         */
        public function buildView(FormView $view, FormInterface $form, array $options)
        {
            if (array_key_exists('image_path', $options)) {
                $parentData = $form->getParent()->getData();

                if (null !== $parentData) {
                    $accessor = PropertyAccess::createPropertyAccessor();
                    $imageUrl = $accessor->getValue($parentData, $options['image_path']);
                } else {
                     $imageUrl = null;
                }

                // set an "image_url" variable that will be available when rendering this field
                $view->vars['image_url'] = $imageUrl;
            }
        }

    }

Nadpisywanie fragmentu szablonu pola File
------------------------------------------

Każdy typ pola renderowany jest za pomocą fragmentu szablonu. Te fragmenty
mogą być nadpisywane w celu dostosowania ich do swojch potrzeb. Aby dowiedzieć się więcej na ten temat,
przeczytaj artykuł :ref:`cookbook-form-customization-form-themes`.

Twoja klasa rozszerzająca typ pola, posiada dodaną nową zmienną (``image_url``), teraz 
potrzebujesz skorzystać z niej w szablonie.
Konkretnie, potrzebujesz nadpisać blok ``file_widget``:

.. configuration-block::

    .. code-block:: html+jinja

        {# src/Acme/DemoBundle/Resources/views/Form/fields.html.twig #}
        {% extends 'form_div_layout.html.twig' %}

        {% block file_widget %}
            {% spaceless %}

            {{ block('form_widget') }}
            {% if image_url is not null %}
                <img src="{{ asset(image_url) }}"/>
            {% endif %}

            {% endspaceless %}
        {% endblock %}

    .. code-block:: html+php

        <!-- src/Acme/DemoBundle/Resources/views/Form/file_widget.html.php -->
        <?php echo $view['form']->widget($form) ?>
        <?php if (null !== $image_url): ?>
            <img src="<?php echo $view['assets']->getUrl($image_url) ?>"/>
        <?php endif ?>

.. note::

    Będziesz potrzebował wprowadzić zmiany w konfiguracji swojej aplikacji, lub jawnie określić
    w jaki sposób Symfony ma nadpisywać bloki szablonu formularzy, aby Twój blok został uwzględniony przy renderownaiu.
    Zobacz: ref:`cookbook-form-customization-form-themes` po więcej informacji.


Używanie rozszerzenia typu pola
-----------------------------

Od teraz, kiedy będziesz dodawał pole ``file`` w formularzu, będziesz miał możliwość
określenia opcji ``image_path``, która będzie wykorzystywana do wyświetlenia obrazka.
Dla przykładu::

    // src/Acme/DemoBundle/Form/Type/MediaType.php
    namespace Acme\DemoBundle\Form\Type;

    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\FormBuilderInterface;

    class MediaType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                ->add('name', 'text')
                ->add('file', 'file', array('image_path' => 'webPath'));
        }

        public function getName()
        {
            return 'media';
        }
    }

Teraz podczas wyświetlania formularza, jeżeli będzie przypisany obrazek,
zostanie wyświetlony obok pola file. 
