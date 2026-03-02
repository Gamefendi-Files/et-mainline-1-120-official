quest sash begin
	state start begin
		when 60003.chat."Kombinasyon" begin
			say("Ýki kuþaðý kombine mi etmek istiyorsun?")
			say("")
			local confirm = select("Evet", "Hayýr")
			if confirm == 2 then
				return
			end
			
			setskin(NOWINDOW)
			pc.open_sash(true)
		end
		
		when 60003.chat."Bonus Emiþi" begin
			say("Silah veya zýrhýndan bonus mu emmek istersin?")
			say("")
			local confirm = select("Evet", "Hayýr")
			if confirm == 2 then
				return
			end
			
			setskin(NOWINDOW)
			pc.open_sash(false)
		end
	end
end