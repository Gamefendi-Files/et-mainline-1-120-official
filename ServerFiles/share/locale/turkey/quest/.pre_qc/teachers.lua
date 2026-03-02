quest skill_group begin
        state start begin
                when login or levelup with pc.level>=5 and pc.skillgroup ==0 begin
                        set_state(run)
                end
        end

        state run begin
                when login with pc.get_skill_group()!=0 begin
                        restart_quest()
                end

                when letter with pc.get_skill_group()==0 begin
                        send_letter("Beceri Eðitimi")

                        local job=pc.get_job()

                        if job==0 then
                                local v=pc_find_skill_teacher_vid(1)
                                if 0!=v then target.vid("teacher1", v, "Bedensel Savas Ögretmeni") end
                                local v=pc_find_skill_teacher_vid(2)
                                if 0!=v then target.vid("teacher2", v, "Zihinsel Savas Ögretmeni") end
                        elseif job==1 then
                                local v=pc_find_skill_teacher_vid(1)
                                if 0!=v then target.vid("teacher3", v, "Yakin Dövüþ Ögretmeni") end
                                local v=pc_find_skill_teacher_vid(2)
                                if 0!=v then target.vid("teacher4", v, "Uzak Dövüþ Ögretmeni") end
                        elseif job==2 then
                                local v=pc_find_skill_teacher_vid(1)
                                if 0!=v then target.vid("teacher5", v, "Büyülü Silah Ögretmeni") end
                                local v=pc_find_skill_teacher_vid(2)
                                if 0!=v then target.vid("teacher6", v, "Kara Büyü Ögretmeni") end
                        elseif job==3 then
                                local v=pc_find_skill_teacher_vid(1)
                                if 0!=v then target.vid("teacher7", v, "Ejderha Gücü Ögretmeni") end
                                local v=pc_find_skill_teacher_vid(2)
                                if 0!=v then target.vid("teacher8", v, "iyilestirme Ögretmeni") end
                        clear_letter()
                        end
                end

                when leave begin
                        target.delete("teacher1")
                        target.delete("teacher2")
                        target.delete("teacher3")
                        target.delete("teacher4")
                        target.delete("teacher5")
                        target.delete("teacher6")
                        target.delete("teacher7")
                        target.delete("teacher8")
                end

                when button or info begin
                        if pc.job == 0 then
                                say_title("Savaþçý Eðitimi Öðretmeni:")
                                say("")
                                say_reward("Eðer hayran olunan bir savaþçý olmak istiyorsan, ")
                                say_reward("hýzlý ve tekrarlanan saldýrý bilgisi veren ")
                                say_reward("Bedensel Savaþý öðrenmelisin. Eðer tüm canavar ")
                                say_reward("gruplarýný bir vuruþta öldürmek istiyorsan, ")
                                say_reward("Zihinsel Savaþ öðrenmelisin. ")
                                say_reward("Köy yakýnlarýnda gelmeni bekleyeceðiz.")
                                say("")
                        elseif pc.job == 1 then
                                say_title("Ninja Eðitimi Öðretmeni:")
                                say("")
                                say_reward("Eðer iyi bir niþancý olarak tanýnmak istiyorsan, ")
                                say_reward("Uzak Dövüþ sanatýný öðren. ")
                                say_reward("Eðer, karanlýðýn korumasýnda ninja olarak ")
                                say_reward("farklý, gizli görevleri baþarmak istiyorsan ")
                                say_reward("Yakýn Dövüþ sanatý senin için daha uygun.")
                                say_reward("Köy yakýnlarýnda gelmeni bekleyeceðiz. ")
                                say("")
                        elseif pc.job == 2 then
                                say_title("Sura Eðitimi Öðretmeni:")
                                say("")
                                say_reward("Ýnsan üstü yetenekleri olan gerçek bir temsilci")
                                say_reward("ve binici olmak mý istiyorsun? Sana Büyülü Silah")
                                say_reward("Öðretmenini öneririm.Fakat,karanlýk büyücü ve")
                                say_reward("lanetler gibi korkulan olmak istiyorsan,")
                                say_reward("sana Kara Büyü Öðretmenini")
                                say_reward("öneririm.")
                                say_reward("Köy yakýnlarýnda gelmeni bekleyeceðiz. ")
                                say("")
                        elseif pc.job == 3 then
                                say_title("Þaman Eðitimi Öðretmeni:")
                                say("")
                                say_reward("Eðer büyü ve kutsamalarý kontrol eden")
                                say_reward("bir þaman olmak istiyorsan ,Ejderha Gücü ")
                                say_reward("Öðretmenine git.Eðer diðerlerinin")
                                say_reward("üzüntülerini ve acýlarýný azaltmak")
                                say_reward("istiyorsan,sana iyileþtirme Öðretmenini")
                                say_reward("öneririm. ")
                                say_reward("Köy yakýnlarýnda gelmeni bekleyeceðiz. ")
                                say("")
                        end
                        ---setleftimage("")

                        select("Onayla")
                        setskin(NOWINDOW)
                end

                -- Arahan Troop
                when teacher1.target.click or
                        skill_group1_1.chat."Bedensel Savaþ eðitimine baþla" or
                        skill_group2_1.chat."Bedensel Savaþ eðitimine baþla" or
                        skill_group3_1.chat."Bedensel Savaþ eðitimine baþla"
                        begin
                        target.delete("teacher1")

                        ----------------------------------------------
                        local pc_job=pc.get_job()
                        if pc_job != 0 then -- !!!
                                ---                                                   l
                                say_title("Yanlýþ Eðitim ")
                                say("")
                                say("Özür dilerim.")
                                say("Sadece Savaþçýlar Bedensel Savaþý seçebilir.")
                                say("")
                                say("")

                                if pc_job==0 then
                                        ---                                                   l
                                        say_reward("Bir Savaþçý,  Zihinsel Savaþý ")
                                        say_reward("veya Bedensel Savaþý ")
                                        say_reward("seçebilir.")
                                elseif pc_job==1 then
                                        say_reward("Bir Ninja, Yakin Dövüþ veya")
                                        say_reward("Uzak Dövüþ eðitimini")
                                        say_reward("seçebilir.")
                                elseif pc_job==2 then
                                        say_reward("Bir Sura, Büyülü Silah")
                                        say_reward("veya Kara Büyü eðitimini")
                                        say_reward("seçebilir.")
                                elseif pc_job==3 then
                                        say_reward("Bir Þaman, Ejderha Gücü eðitimini")
                                        say_reward("veya Ýyileþtirme eðitimini")
                                        say_reward("seçebilir.")
                                end
                                say("")
                                return
                        end
                        ----------------------------------------------

                        say_title("Bedensel Savaþ eðitimi")
                        ---                                                   l
                        say("Bedensel savaþ eðitimi hýzlý savaþma eðitimi ve")
                        say("metodlarýný, ayrýca geliþmiþ temel saldýrý ")
                        say("tiplerini içerir.")
                        say_reward("Gereksinim: Seviye 5 ve üstü bir savaþçý olmak")
                        say("")


                        ----------------------------------------------
                        local s=select("Baþla", "Daha sonra")

                        if 2 == s then
                                return
                        end

                        if pc.get_job()!=0 or pc.get_skill_group()!=0 then
                                test_chat("npc_is_same_job:"..bool_to_str(npc_is_same_job()))
                                test_chat("pc.get_skill_group:"..pc.get_skill_group())
                                test_chat("pc_job:"..pc.get_job())
                                return
                        end

                        set_state("start")
                        pc.set_skill_group(1) -- !!!
                        pc.clear_skill()
                        char_log(0, "CLEAR_SKILL", "Ögretmen silindikten sonraki dogal yetenek.")
                        ----------------------------------------------

                        say_title("Bedensel savaþ eðitimini seçtin.")
                        ---                                                   l
                        say("Hoþgeldin!")

                        if not pc.is_clear_skill_group() then
                                ---                                                   l
                                say("Ders almayý seçtiðin için becerilerini ")
                                say("normal olarak veriyorum")
                                say("Geliþtirmek için beceri kitabýna ihtiyacin olacak.")
                                say_title("Bilgi:")
                                say_reward("Becerilerin normal Olarak Verildi.")
                                say("")
								pc.set_skill_level (124,59)--		madencilik
								pc.set_skill_level (130,59)--		at çaðýrma
								pc.set_skill_level (131,59)--		binicilik
								pc.set_skill_level (121,59)--		liderlik
								pc.set_skill_level (129,59)--		dönüþüm
								pc.set_skill_level (126,59)--		lisan
								pc.set_skill_level (127,59)--		lisan
								pc.set_skill_level (128,59)--		lisan
								pc.set_skill_level (137,59)--		at skili
								pc.set_skill_level (138,59)--		at skili
								pc.set_skill_level (139,59)--		at skili
								pc.set_skill_level (122,2)

                        else
                                say_reward("Yeni bir ders seçtiginde,")
                                say_reward("daha fazla tecrübe puani almayacaksin.")
                                say("")
                        end
                        clear_letter()
                end

                -- Byulki Troop
                when teacher2.target.click or
                        skill_group1_2.chat."Zihinsel-Savaþ eðitimine baþla" or
                        skill_group2_2.chat."Zihinsel-Savaþ eðitimine baþla" or
                        skill_group3_2.chat."Zihinsel-Savaþ eðitimine baþla"
                        begin
                        target.delete("teacher2")

                        ----------------------------------------------
                        local pc_job=pc.get_job()
                        if pc_job != 0 then
                                ---                                                   l
                                say_title("Yanlýþ Eðitim ")
                                say("")
                                say("Özür Dilerim.")
                                say("Sadece Savaþçýlar Zihinsel Savaþý ")
                                say("seçebilir.")
                                say("")

                                if pc_job==0 then
                                        ---                                                   l
                                        say_reward("Bir Savaþçý , Zihinsel Savaþý ")
                                        say_reward("veya Bedensel Savaþý ")
                                        say_reward("seçebilir.")
                                elseif pc_job==1 then
                                        say_reward("Bir Ninja, Yakýn Dövüþ veya")
                                        say_reward("Uzak Dövüþ eðitimini")
                                        say_reward("seçebilir.")
                                elseif pc_job==2 then
                                        say_reward("Bir Sura, Büyülü Silah")
                                        say_reward("veya Kara Büyü eðitimini")
                                        say_reward("seçebilir.")
                                elseif pc_job==3 then
                                        say_reward("Bir saman ,Ejderha Gücü eðitimini")
                                        say_reward("veya iyilestirme eðitimini")
                                        say_reward("seçebilir.")
                                end
                                say("")
                                return
                        end
                        ----------------------------------------------

                        say_title("Zihinsel Savaþ eðitimi")
                        say("")
                        ---                                                   l
                        say("Zihinsel savaþ eðitimi sana düþmanlarýna nasýl")
                        say("güçlü chi-saldýrýlarý yapabileceðini ve kendini")
                        say("düþman saldýrýlarýndan koruyabileceðini öðretir.")
                        say_reward("Gereksinim: Seviye 5 ve üstü bir savaþçý olmak")
                        say("")

                        ----------------------------------------------
                        local s=select("Baþla", "Daha sonra")

                        if 2 == s then
                                return
                        end

                        if pc.get_job()!=0 or pc.get_skill_group()!=0 then
                                test_chat("npc_is_same_job:"..bool_to_str(npc_is_same_job()))
                                test_chat("pc.get_skill_group:"..pc.get_skill_group())
                                test_chat("pc_job:"..pc.get_job())
                                return
                        end

                        set_state("start")
                        pc.set_skill_group(2)
                        pc.clear_skill()
                        char_log(0, "CLEAR_SKILL", "Ögretmen silindikten sonraki dogal yetenek")
                        ----------------------------------------------


                        say_title("Zihinsel savaþ eðitimini seçtin.")
                        ---                                                   l
                        say("Hoþgeldin!")

                        if not pc.is_clear_skill_group() then
                                           ---                                                   l
								say("Ders almayý seçtiðin için becerilerini ")
                                say("normal olarak veriyorum")
                                say("Geliþtirmek için beceri kitabýna ihtiyacin olacak.")
                                say_title("Bilgi:")
                                say_reward("Becerilerin normal Olarak Verildi.")
                                say("")
								pc.set_skill_level (124,59)--		madencilik
								pc.set_skill_level (130,59)--		at çaðýrma
								pc.set_skill_level (131,59)--		binicilik
								pc.set_skill_level (121,59)--		liderlik
								pc.set_skill_level (129,59)--		dönüþüm
								pc.set_skill_level (126,59)--		lisan
								pc.set_skill_level (127,59)--		lisan
								pc.set_skill_level (128,59)--		lisan
								pc.set_skill_level (137,59)--		at skili
								pc.set_skill_level (138,59)--		at skili
								pc.set_skill_level (139,59)--		at skili
								pc.set_skill_level (122,2)

                        else
                                say_reward("Yeni bir ders seçtiginde,")
                                say_reward("daha fazla tecrübe puani almayacaksin.")
                                say("")
                        end
                        clear_letter()
                end


                -- Bisal Troop
                when teacher3.target.click or
                        skill_group1_3.chat."Yakin Dövüþ eðitimine baþla" or
                        skill_group2_3.chat."Yakin Dövüþ eðitimine baþla" or
                        skill_group3_3.chat."Yakin Dövüþ eðitimine baþla"
                        begin
                        target.delete("teacher3")


                        ----------------------------------------------
                        local pc_job=pc.get_job()
                        if pc_job != 1 then -- !!!
                                ---                                                   l
                                say_title("Yanlýþ Eðitim ")
                                say("")
                                say("Özür Dilerim.")
                                say("Sadece Ninjalar Yakýn Dövüþ eðitimini seçebilir.")
                                say("")

                                if pc_job==0 then
                                        ---                                                   l
                                        say_reward("Bir Savaþçý,  Zihinsel Savaþý ")
                                        say_reward("veya Bedensel Savaþý ")
                                        say_reward("seçebilir.")
                                elseif pc_job==1 then
                                        say_reward("Bir Ninja ,Yakýn Dövüþ veya")
                                        say_reward("Uzak Dövüþ eðitimini")
                                        say_reward("seçebilir.")
                                elseif pc_job==2 then
                                        say_reward("Bir Sura, Büyülü Silah")
                                        say_reward("veya Kara Büyü eðitimini")
                                        say_reward("seçebilir.")
                                elseif pc_job==3 then
                                        say_reward("Bir saman, Ejderha Gücü eðitimini")
                                        say_reward("veya iyilestirme eðitimini")
                                        say_reward("seçebilir.")
                                end
                                say("")
                                return
                        end
                        ----------------------------------------------
                        say_title("Yakýn Dövüþ eðitimi")
                        say("")
                        ---                                                   l
                        say("Yakin Dövüþ eðitiminde ,Ninjalara düþmanlarýnýn ")
                        say("zayýf noktalarýna , nasýl hýzlý ve ölümcül")
                        say("vuruþlar yapabilecekleri öðretilir.")
                        say_reward("Gereksinim: Seviye 5 ve üstü bir savaþçý olmak")
                        say("")

                        ----------------------------------------------
                        local s=select("Baþla", "Daha Sonra")

                        if 2 == s then
                                return
                        end

                        if pc.get_job()!=1 or pc.get_skill_group()!=0 then
                                test_chat("npc_is_same_job:"..bool_to_str(npc_is_same_job()))
                                test_chat("pc.get_skill_group:"..pc.get_skill_group())
                                test_chat("pc_job:"..pc.get_job())
                                return
                        end

                        set_state("start")
                        pc.set_skill_group(1) -- !!!
                        pc.clear_skill()
                        char_log(0, "CLEAR_SKILL", "Ögretmen silindikten sonraki dogal yetenek")
                        ----------------------------------------------

                        say_title("Yakýn Dövüþ eðitimini seçtin.")
                        ---                                                   l
                        say("Hoþgeldin!")

                        if not pc.is_clear_skill_group() then
                                ---                                                   l
								say("Ders almayý seçtiðin için becerilerini ")
                                say("normal olarak veriyorum")
                                say("Geliþtirmek için beceri kitabýna ihtiyacin olacak.")
                                say_title("Bilgi:")
                                say_reward("Becerilerin normal Olarak Verildi.")
                                say("")
								pc.set_skill_level (124,59)--		madencilik
								pc.set_skill_level (130,59)--		at çaðýrma
								pc.set_skill_level (131,59)--		binicilik
								pc.set_skill_level (121,59)--		liderlik
								pc.set_skill_level (129,59)--		dönüþüm
								pc.set_skill_level (126,59)--		lisan
								pc.set_skill_level (127,59)--		lisan
								pc.set_skill_level (128,59)--		lisan
								pc.set_skill_level (137,59)--		at skili
								pc.set_skill_level (138,59)--		at skili
								pc.set_skill_level (139,59)--		at skili
								pc.set_skill_level (122,2)
								pc.set_skill_level (140,59)

                        else
                                say_reward("Yeni bir ders seçtiðinde,")
                                say_reward("daha fazla tecrübe puaný almayacaksýn.")
                                say("")
                        end
                        clear_letter()
                end


                -- Kangno Troop
                when teacher4.target.click or
                        skill_group1_4.chat."Yakýn Dövüþ eðitimine baþla" or
                        skill_group2_4.chat."Yakýn Dövüþ eðitimine baþla" or
                        skill_group3_4.chat."Yakýn Dövüþ eðitimine baþla"
                        begin
                        target.delete("teacher4")

                        ----------------------------------------------
                        local pc_job=pc.get_job()
                        if pc_job != 1 then -- !!!
                                ---                                                   l
                                say_title("Yanlýþ Eðitim ")
                                say("")
                                say("Özür Dilerim.")
                                say("Sadece Ninjalar Uzak Dövüþ eðitimini seçebilir.")
                                say("")

                                if pc_job==0 then
                                        ---                                                   l
                                        say_reward("Bir Savaþçý,  Zihinsel Savaþý ")
                                        say_reward("veya Bedensel Savaþý ")
                                        say_reward("seçebilir.")
                                elseif pc_job==1 then
                                        say_reward("Bir Ninja, Yakýn Dövüþ veya")
                                        say_reward("Uzak Dövüþ eðitimini")
                                        say_reward("seçebilir.")
                                elseif pc_job==2 then
                                        say_reward("Bir Sura, Büyülü Silah")
                                        say_reward("veya Kara Büyü eðitimini")
                                        say_reward("seçebilir.")
                                elseif pc_job==3 then
                                        say_reward("Bir saman ,Ejderha Gücü eðitimini")
                                        say_reward("veya Ýyileþtirme eðitimini")
                                        say_reward("seçebilir.")
                                end
                                say("")
                                return
                        end
                        ----------------------------------------------

                        say_title("Uzak Dövüþ eðitimi")
                        say("")
                        ---                                                   l
                        say("Uzak Dövüþ eðitiminde ,Ninjalara nasýl ok")
                        say("kullanmalarý gerektiði ve uzak mesafeden")
                        say("düþmanlarýyla dikkatli savaþmalarý öðretilir.")
                        say_reward("Gereksinim: Seviye 5 ve üstü bir savaþçý olmak")
                        say("")

                        ----------------------------------------------
                        local s=select("Baþla", "Daha sonra")

                        if 2 == s then
                                return
                        end

                        if pc.get_job()!=1 or pc.get_skill_group()!=0 then
                                test_chat("npc_is_same_job:"..bool_to_str(npc_is_same_job()))
                                test_chat("pc.get_skill_group:"..pc.get_skill_group())
                                test_chat("pc_job:"..pc.get_job())
                                return
                        end

                        set_state("start")
                        pc.set_skill_group(2) -- !!!
                        pc.clear_skill()
                        char_log(0, "CLEAR_SKILL", "Ögretmen silindikten sonraki dogal yetenek")
                        ----------------------------------------------

                        say_title("Uzak Dövüþ eðitimini seçtin.")
                        ---                                                   l
                        say("Hoþgeldin!")

                        if not pc.is_clear_skill_group() then
                                           ---                                                   l
								say("Ders almayý seçtiðin için becerilerini ")
                                say("normal olarak veriyorum")
                                say("Geliþtirmek için beceri kitabýna ihtiyacin olacak.")
                                say_title("Bilgi:")
                                say_reward("Becerilerin normal Olarak Verildi.")
                                say("")
								pc.set_skill_level (124,59)--		madencilik
								pc.set_skill_level (130,59)--		at çaðýrma
								pc.set_skill_level (131,59)--		binicilik
								pc.set_skill_level (121,59)--		liderlik
								pc.set_skill_level (129,59)--		dönüþüm
								pc.set_skill_level (126,59)--		lisan
								pc.set_skill_level (127,59)--		lisan
								pc.set_skill_level (128,59)--		lisan
								pc.set_skill_level (137,59)--		at skili
								pc.set_skill_level (138,59)--		at skili
								pc.set_skill_level (139,59)--		at skili
								pc.set_skill_level (122,2)
								pc.set_skill_level (140,59)

                        else
                                say_reward("Yeni bir ders seçtiðinde,")
                                say_reward("daha fazla tecrübe puaný almayacaksýn.")
                                say("")
                        end
                        clear_letter()
                end


                -- Hwanmoo Troop
                when teacher5.target.click or
                        skill_group1_5.chat."Büyülü Silah eðitimine baþla" or
                        skill_group2_5.chat."Büyülü Silah eðitimine baþla" or
                        skill_group3_5.chat."Büyülü Silah eðitimine baþla"
                        begin
                        target.delete("teacher5")

                        ----------------------------------------------
                        local pc_job=pc.get_job()
                        if pc_job != 2 then -- !!!
                                ---                                                   l
                                say_title("Yanlýþ Eðitim ")
                                say("")
                                say("Özür Dilerim.")
                                say("Sadece Suralar Büyülü Silah eðitimini")
                                say("seçebilir.")
                                say("")

                                if pc_job==0 then
                                        ---                                                   l
                                        say_reward("Bir Savaþçý,  Zihinsel Savaþý ")
                                        say_reward("veya Bedensel Savaþý ")
                                        say_reward("seçebilir.")
                                elseif pc_job==1 then
                                        say_reward("Bir Ninja, Yakýn Dövüþ veya")
                                        say_reward("Uzak Dövüþ eðitimini")
                                        say_reward("seçebilir.")
                                elseif pc_job==2 then
                                        say_reward("Bir Sura, Büyülü Silah")
                                        say_reward("veya Kara Büyü eðitimini")
                                        say_reward("seçebilir.")
                                elseif pc_job==3 then
                                        say_reward("Bir saman, Ejderha Gücü eðitimini")
                                        say_reward("veya Ýyileþtirme eðitimini")
                                        say_reward("seçebilir.")
                                end
                                say("")
                                return
                        end
                        ----------------------------------------------

                        say_title("Büyülü Silah eðitimi")
                        say("")
                        ---                                                   l
                        say("Büyü Silah eðitiminde ,zayýflatýlmýþ düþmanlara")
                        say("karþý ,lanetli býçaklarý kullanarak güçlü ")
                        say("darbe vurma yöntemleri öðretilir.")
                        say_reward("Gereksinim: Seviye 5 ve üstü bir savaþçý olmak")
                        say("")

                        ----------------------------------------------
                        local s=select("Baþla", "Daha sonra")

                        if 2 == s then
                                return
                        end

                        if pc.get_job()!=2 or pc.get_skill_group()!=0 then
                                test_chat("npc_is_same_job:"..bool_to_str(npc_is_same_job()))
                                test_chat("pc.get_skill_group:"..pc.get_skill_group())
                                test_chat("pc_job:"..pc.get_job())
                                return
                        end

                        set_state("start")
                        pc.set_skill_group(1) -- !!!
                        pc.clear_skill()
                        char_log(0, "CLEAR_SKILL", "Ögretmen silindikten sonraki dogal yetenek")
                        ----------------------------------------------


                        say_title("Büyülü Silah eðitimini seçtin.")
                        ---                                                   l
                        say("Hoþgeldin!")

                        if not pc.is_clear_skill_group() then
                                           ---                                                   l
								say("Ders almayý seçtiðin için becerilerini ")
                                say("normal olarak veriyorum")
                                say("Geliþtirmek için beceri kitabýna ihtiyacin olacak.")
                                say_title("Bilgi:")
                                say_reward("Becerilerin normal Olarak Verildi.")
                                say("")
								pc.set_skill_level (124,59)--		madencilik
								pc.set_skill_level (130,59)--		at çaðýrma
								pc.set_skill_level (131,59)--		binicilik
								pc.set_skill_level (121,59)--		liderlik
								pc.set_skill_level (129,59)--		dönüþüm
								pc.set_skill_level (126,59)--		lisan
								pc.set_skill_level (127,59)--		lisan
								pc.set_skill_level (128,59)--		lisan
								pc.set_skill_level (137,59)--		at skili
								pc.set_skill_level (138,59)--		at skili
								pc.set_skill_level (139,59)--		at skili
								pc.set_skill_level (122,2)

                        else
                                say_reward("Yeni bir ders seçtiginde,")
                                say_reward("daha fazla tecrübe puani almayacaksin.")
                                say("")
                        end
                        clear_letter()
                end


                -- Black Horse Troop
                when teacher6.target.click or
                        skill_group1_6.chat."Kara Büyü eðitimine baþla" or
                        skill_group2_6.chat."Kara Büyü eðitimine baþla" or
                        skill_group3_6.chat."Kara Büyü eðitimine baþla"
                        begin
                        target.delete("teacher6")

                        ----------------------------------------------
                        local pc_job=pc.get_job()
                        if pc_job != 2 then -- !!!
                                ---                                                   l
                                say_title("Yanlýþ Eðitim ")
                                say("")
                                say("Özür Dilerim.")
                                say("Sadece Suralar Kara Büyü eðitimini")
                                say("seçebilir.")
                                say("")

                                if pc_job==0 then
                                        ---                                                   l
                                        say_reward("Bir Savaþçý,  Zihinsel Savaþý ")
                                        say_reward("veya Bedensel Savaþý ")
                                        say_reward("seçebilir.")
                                elseif pc_job==1 then
                                        say_reward("Bir Ninja, Yakýn Dövüþ veya")
                                        say_reward("Uzak Dövüþ eðitimini")
                                        say_reward("seçebilir.")
                                elseif pc_job==2 then
                                        say_reward("Bir Sura, Büyülü Silah")
                                        say_reward("veya Kara Büyü eðitimini")
                                        say_reward("seçebilir.")
                                elseif pc_job==3 then
                                        say_reward("Bir saman, Ejderha Gücü eðitimini")
                                        say_reward("veya Ýyileþtirme eðitimini")
                                        say_reward("seçebilir.")
                                end
                                say("")
                                return
                        end
                        ----------------------------------------------

                        say_title("Kara Büyü eðitimi")
                        say("")
                        ---                                                   l
                        say("Kara Büyü eðitiminde, uzak mesafeden ")
                        say("düsmanlarini zayif düsüren  lanet yöntemleri")
                        say("ögretilir.")
                        say_reward("Gereksinim: Seviye 5 ve üstü bir savaþçý olmak")
                        say("")

                        ----------------------------------------------
                        local s=select("Baþla", "Daha sonra")

                        if 2 == s then
                                return
                        end

                        if pc.get_job()!=2 or pc.get_skill_group()!=0 then
                                test_chat("npc_is_same_job:"..bool_to_str(npc_is_same_job()))
                                test_chat("pc.get_skill_group:"..pc.get_skill_group())
                                test_chat("pc_job:"..pc.get_job())
                                return
                        end

                        set_state("start")
                        pc.set_skill_group(2) -- !!!
                        pc.clear_skill()
                        char_log(0, "CLEAR_SKILL", "Ögretmen silindikten sonraki dogal yetenek.")
                        ----------------------------------------------

                        say_title("Kara Büyü eðitimini seçtin.")
                        ---                                                   l
                        say("Hoþgeldin!")

                        if not pc.is_clear_skill_group() then
                                           ---                                                   l
								say("Ders almayý seçtiðin için becerilerini ")
                                say("normal olarak veriyorum")
                                say("Geliþtirmek için beceri kitabýna ihtiyacin olacak.")
                                say_title("Bilgi:")
                                say_reward("Becerilerin normal Olarak Verildi.")
                                say("")
								pc.set_skill_level (124,59)--		madencilik
								pc.set_skill_level (130,59)--		at çaðýrma
								pc.set_skill_level (131,59)--		binicilik
								pc.set_skill_level (121,59)--		liderlik
								pc.set_skill_level (129,59)--		dönüþüm
								pc.set_skill_level (126,59)--		lisan
								pc.set_skill_level (127,59)--		lisan
								pc.set_skill_level (128,59)--		lisan
								pc.set_skill_level (137,59)--		at skili
								pc.set_skill_level (138,59)--		at skili
								pc.set_skill_level (139,59)--		at skili
								pc.set_skill_level (122,2)

                        else
                                say_reward("Yeni bir ders seçtiginde,")
                                say_reward("daha fazla tecrübe puani almayacaksin.")
                                say("")
                        end
                        clear_letter()
                end


                -- Heavenly Dragon Troop
                when teacher7.target.click or
                        skill_group1_7.chat."Ejderha Gücü eðitimine baþla" or
                        skill_group2_7.chat."Ejderha Gücü eðitimine baþla" or
                        skill_group3_7.chat."Ejderha Gücü eðitimine baþla"
                        begin
                        target.delete("teacher7")

                        ----------------------------------------------
                        local pc_job=pc.get_job()
                        if pc_job != 3 then -- !!!
                                ---                                                   l
                                say_title("Yanlýþ Eðitim ")
                                say("")
                                say("Özür Dilerim.")
                                say("Sadece samanlar Ejderha Gücü eðitimini")
                                say("seçebilir.")
                                say("")

                                if pc_job==0 then
                                        ---                                                   l
                                        say_reward("Bir Savaþçý, Zihinsel Savaþý ")
                                        say_reward("veya Bedensel Savaþý ")
                                        say_reward("seçebilir.")
                                elseif pc_job==1 then
                                        say_reward("Bir Ninja, Yakin Dövüþ veya")
                                        say_reward("Uzak Dövüþ eðitimini")
                                        say_reward("seçebilir.")
                                elseif pc_job==2 then
                                        say_reward("Bir Sura, Büyülü Silah")
                                        say_reward("veya Kara Büyü eðitimini")
                                        say_reward("seçebilir.")
                                elseif pc_job==3 then
                                        say_reward("Bir saman, Ejderha Gücü eðitimini")
                                        say_reward("veya iyilestirme eðitimini")
                                        say_reward("seçebilir.")
                                end
                                say("")
                                return
                        end
                        ----------------------------------------------

                        say_title("Ejderha Gücü eðitimi")
                        say("")
                        ---                                                   l
                        say("Ejderha Gücü eðitiminde, Ejderha Tanrýnýn ")
                        say("güçlü ateþ saldýrýlarýný yönetmeyi ve ")
                        say("dayanýklýlýk büyüleriyle Ejderha Tanrýnýn ")
                        say("korumasýný kullanmayi ögrenirler.")
                        say_reward("Gereksinim: Seviye 5 ve üstü bir savaþçý olmak")
                        say("")

                        ----------------------------------------------
                        local s=select("Baþla", "Daha sonra")

                        if 2 == s then
                                return
                        end

                        if pc.get_job()!=3 or pc.get_skill_group()!=0 then
                                test_chat("npc_is_same_job:"..bool_to_str(npc_is_same_job()))
                                test_chat("pc.get_skill_group:"..pc.get_skill_group())
                                test_chat("pc_job:"..pc.get_job())
                                return
                        end

                        set_state("start")
                        pc.set_skill_group(1) -- !!!
                        pc.clear_skill()
                        char_log(0, "CLEAR_SKILL", "Ögretmen silindikten sonraki dogal yetenek.")
                        ----------------------------------------------

                        say_title("Ejderha Gücü eðitimini seçtin.")
                        ---                                                   l
                        say("Hoþgeldin!")

                        if not pc.is_clear_skill_group() then
                                           ---                                                   l
								say("Ders almayý seçtiðin için becerilerini ")
                                say("normal olarak veriyorum")
                                say("Geliþtirmek için beceri kitabýna ihtiyacin olacak.")
                                say_title("Bilgi:")
                                say_reward("Becerilerin normal Olarak Verildi.")
                                say("")
								pc.set_skill_level (124,59)--		madencilik
								pc.set_skill_level (130,59)--		at çaðýrma
								pc.set_skill_level (131,59)--		binicilik
								pc.set_skill_level (121,59)--		liderlik
								pc.set_skill_level (129,59)--		dönüþüm
								pc.set_skill_level (126,59)--		lisan
								pc.set_skill_level (127,59)--		lisan
								pc.set_skill_level (128,59)--		lisan
								pc.set_skill_level (137,59)--		at skili
								pc.set_skill_level (138,59)--		at skili
								pc.set_skill_level (139,59)--		at skili
								pc.set_skill_level (122,2)

                        else
                                say_reward("Yeni bir ders seçtiginde,")
                                say_reward("daha fazla tecrübe puani almayacaksin.")
                                say("")
                        end
                        clear_letter()
                end


                -- Thunder Light Troop
                when teacher8.target.click or
                        skill_group1_8.chat."Ýyileþtirme eðitimine baþla" or
                        skill_group2_8.chat."Ýyileþtirme eðitimine baþla" or
                        skill_group3_8.chat."Ýyileþtirme eðitimine baþla"
                        begin
                        target.delete("teacher8")

                        ----------------------------------------------
                        local pc_job=pc.get_job()
                        if pc_job != 3 then -- !!!
                                ---                                                   l
                                say_title("Yanlýþ Eðitim ")
                                say("")
                                say("Özür Dilerim.")
                                say("Sadece Þamanlar Ýyileþtirme Eðitimi")
                                say("seçebilir.")
                                say("")

                                if pc_job==0 then
                                        ---                                                   l
                                        say_reward("Bir Savaþçý,  Zihinsel Savaþý ")
                                        say_reward("veya Bedensel Savaþý ")
                                        say_reward("seçebilir.")
                                elseif pc_job==1 then
                                        say_reward("Bir Ninja, Yakýn Dövüþ veya")
                                        say_reward("Uzak Dövüþ eðitimini")
                                        say_reward("seçebilir.")
                                elseif pc_job==2 then
                                        say_reward("Bir Sura, Büyülü Silah")
                                        say_reward("veya Kara Büyü eðitimini")
                                        say_reward("seçebilir.")
                                elseif pc_job==3 then
                                        say_reward("Bir saman ,Ejderha Gücü eðitimini")
                                        say_reward("veya Ýyileþtirme eðitimini")
                                        say_reward("seçebilir.")
                                end
                                say("")
                                return
                        end

                        ----------------------------------------------

                        say_title("Ýyileþtirme eðitimi")
                        say("")
                        ---                                                   l
                        say("Ýyileþtirme eðitiminde, düþmanlarýný ")
                        say("uyuþturmayý, ve kendi grup üyelerini")
                        say("iyileþtirerek güçlendirmeyi öðrenirler.")
                        say_reward("Gereksinim: Seviye 5 ve üstü bir savaþçý olmak")
                        say("")

                        ----------------------------------------------
                        local s=select("Baþla", "Daha sonra")

                        if 2 == s then
                                return
                        end

                        if pc.get_job()!=3 or pc.get_skill_group()!=0 then
                                test_chat("npc_is_same_job:"..bool_to_str(npc_is_same_job()))
                                test_chat("pc.get_skill_group:"..pc.get_skill_group())
                                test_chat("pc_job:"..pc.get_job())
                                return
                        end

                        set_state("start")
                        pc.set_skill_group(2) -- !!!
                        pc.clear_skill()
                        char_log(0, "CLEAR_SKILL", "Ögretmen silindikten sonraki dogal yetenek.")
                        ----------------------------------------------

                        say_title("Ýyileþtirme eðitimini seçtin.")
                        ---                                                   l
                        say("Hoþgeldin!")

                        if not pc.is_clear_skill_group() then
                                           ---                                                   l
								say("Ders almayý seçtiðin için becerilerini ")
                                say("normal olarak veriyorum")
                                say("Geliþtirmek için beceri kitabýna ihtiyacin olacak.")
                                say_title("Bilgi:")
                                say_reward("Becerilerin normal Olarak Verildi.")
                                say("")
								pc.set_skill_level (124,59)--		madencilik
								pc.set_skill_level (130,59)--		at çaðýrma
								pc.set_skill_level (131,59)--		binicilik
								pc.set_skill_level (121,59)--		liderlik
								pc.set_skill_level (129,59)--		dönüþüm
								pc.set_skill_level (126,59)--		lisan
								pc.set_skill_level (127,59)--		lisan
								pc.set_skill_level (128,59)--		lisan
								pc.set_skill_level (137,59)--		at skili
								pc.set_skill_level (138,59)--		at skili
								pc.set_skill_level (139,59)--		at skili
								pc.set_skill_level (122,2)

                        else
                                say_reward("Yeni bir ders seçtiginde,")
                                say_reward("daha fazla tecrübe puani almayacaksin.")
                                say("")
                        end
                        clear_letter()
                end

        end -- end_of_state: run
end -- end_of_quest
