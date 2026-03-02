quest yohara_system begin
	state start begin
		when login or levelup with pc.get_level() >= 120 begin
			set_state("information_new")
		end
	end
	
	state information_new begin
		when letter begin
			local v = find_npc_by_vnum(20011)
			if v != 0 then
				target.vid("heykel", v, "heykel")
			end
			
			send_letter("Savaþçý Heykeli ")
		end
		
		when button or info begin
			say_title("Bir þampiyon olmak ")
			say("Savaþçý Heykeli seni görmek istiyor. ")
			say("Onu ziyaret etsen iyi olabilir. ")
			say("Kendisini Liman þehrinde bulabilirsin. ")
			say("")
			say_reward("Mini Harita'daki yanýp sönen noktayý takip et. ")
		end

		when heykel.target.click or 20011.chat."Bir þampiyon olmak " begin
			target.delete("heykel")
			
			say_title("Savaþçý Heykeli: ")
			say("")
			say("Tebrikler.. ")
			say("Hidra'yý maðlup ettin. Artýk gücünü kanýtladýn ")
			say("þampiyon seviyesine geçiþ yapabilirsin. ")
			pc.set_conquerorlevel(1)
			set_state("__COMPLETE__")
		end

	end
	state __COMPLETE__ begin -- Görevi sildirdik
	end
end
